#define STB_SPRINTF_IMPLEMENTATION
#include <stb/stb_sprintf.h>

struct utf8_stream_reader
{
    const uint8_t* Str;
    size_t         Size;
    size_t         Used;
    
    bool  Is_Valid() const;
    uint32_t Consume();
};

struct utf16_stream_writer
{
    uint16_t* Str;
    size_t    Capacity;
    size_t    Size;
    
    void Write(uint32_t Codepoint);
    str16 To_Str16(bool NullTerminate=true);
};

utf8_stream_reader UTF8_Stream_Reader(const uint8_t* Str, size_t Length)
{
    utf8_stream_reader Reader = {};
    Reader.Str = Str;
    Reader.Size = Length;
    return Reader;
}

utf8_stream_reader UTF8_Stream_Reader(const str8& Str)
{
    return UTF8_Stream_Reader(Str.Str, Str.Length);
}

utf8_stream_reader UTF8_Stream_Reader(const uint8_t* Str)
{
    return UTF8_Stream_Reader(Str, Str8_Length(Str));
}

static const uint8_t GClassUTF8[32] = 
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

bool utf8_stream_reader::Is_Valid() const
{
    return Used < Size;
}

uint32_t utf8_stream_reader::Consume()
{
    uint32_t Result = 0xFFFFFFFF;
    
    uint8_t Byte = Str[Used];
    uint8_t ByteClass = GClassUTF8[Byte >> 3];
    Assert((Used+ByteClass) <= Size);
    
    switch(ByteClass)
    {
        case 1:
        {
            Result = Byte;
        } break;
        
        case 2:
        {
            uint8_t NextByte = Str[Used+1];
            if(GClassUTF8[NextByte >> 3] == 0)
            {
                Result = (Byte & Bitmask_5) << 6;
                Result |= (NextByte & Bitmask_6);
            }
        } break;
        
        case 3:
        {
            uint8_t NextBytes[2] = {Str[Used+1], Str[Used+2]};
            if(GClassUTF8[NextBytes[0] >> 3] == 0 &&
               GClassUTF8[NextBytes[1] >> 3] == 0)
            {
                Result = (Byte & Bitmask_4) << 12;
                Result |= ((NextBytes[0] & Bitmask_6) << 6);
                Result |= (NextBytes[1] & Bitmask_6);
            }
        } break;
        
        case 4:
        {
            uint32_t NextBytes[3] = {Str[Used+1], Str[Used+2], Str[Used+3]};
            if(GClassUTF8[NextBytes[0] >> 3] == 0 &&
               GClassUTF8[NextBytes[1] >> 3] == 0 &&
               GClassUTF8[NextBytes[2] >> 3] == 0)
            {
                Result = (Byte & Bitmask_3) << 18;
                Result |= ((NextBytes[0] & Bitmask_6) << 12);
                Result |= ((NextBytes[1] & Bitmask_6) << 6);
                Result |= (NextBytes[2] & Bitmask_6);
            }
        } break;
    }
    
    Used += ByteClass;
    return Result;
}

utf16_stream_writer UTF16_Stream_Writer(uint16_t* Buffer, size_t Capacity)
{
    utf16_stream_writer Writer = {};
    Writer.Str = Buffer;
    Writer.Capacity = Capacity;
    return Writer;
}

utf16_stream_writer UTF16_Stream_Writer(allocator* Allocator, size_t Capacity)
{
    uint16_t* Result = (uint16_t*)Allocator->MemoryAllocate(sizeof(uint16_t)*Capacity, Allocator->UserData);
    Memory_Clear(Result, sizeof(uint16_t)*Capacity);
    return UTF16_Stream_Writer(Result, Capacity);
}

void utf16_stream_writer::Write(uint32_t Codepoint)
{
    Assert(Codepoint != 0xFFFFFFFF);
    
    uint16_t* At = Str + Size;
    if(Codepoint == 0xFFFFFFFF)
    {
        Assert(false);
        At[0] = (uint16_t)'?';
    }
    else if(Codepoint < 0x10000)
    {
        At[0] = (uint16_t)Codepoint;
    }
    else
    {
        Codepoint -= 0x10000;
        At[0] = 0xD800 + (uint16_t)(Codepoint >> 10);
        At[1] = 0xDC00 + (Codepoint & Bitmask_10);
        Size++;
    }
    Size++;
    Assert(Size <= Capacity);
}

str16 utf16_stream_writer::To_Str16(bool NullTerminate)
{
    if(NullTerminate) 
    {
        Assert(Size+1 <= Capacity);
        Str[Size] = 0;
    }
    return Str16(Str, Size);
}

const uint8_t& str8::operator[](size_t Index) const
{
    Assert(Index < Length);
    return Str[Index];
}

size_t Str8_Length(const uint8_t* Str)
{
    size_t Result = 0;
    while(*Str++) Result++;
    return Result;
}

str8 Str8(const uint8_t* Str, size_t Length)
{
    str8 Result = {};
    Result.Str = Str;
    Result.Length = Length;
    return Result;
}

str8 Str8(const uint8_t* Str)
{
    return Str8(Str, Str8_Length(Str));
}

str8 Str8_FormatV(allocator* Allocator, const uint8_t* Format, va_list Args)
{
    static char TempBuffer[1];
    uint32_t Length = stbsp_vsnprintf(TempBuffer, 1, (const char*)Format, Args);
    uint8_t* Buffer = (uint8_t*)Allocator->MemoryAllocate(sizeof(uint8_t)*(Length+1), Allocator->UserData);
    stbsp_vsnprintf((char*)Buffer, sizeof(uint8_t)*(Length+1), (const char*)Format, Args);
    return Str8((uint8_t*)Buffer, Length);
}

str8 Str8_Format(allocator* Allocator, const uint8_t* Format, ...)
{
    va_list List;
    va_start(List, Format);
    str8 Result = Str8_FormatV(Allocator, Format, List);
    va_end(List);
    return Result;
}

str8 Str8_FormatV(allocator* Allocator, const str8& Str, va_list Args)
{
    return Str8_FormatV(Allocator, Str.Str, Args);
}

str8 Str8_Format(allocator* Allocator, const str8& Str, ...)
{
    va_list List;
    va_start(List, Str.Str);
    str8 Result = Str8_FormatV(Allocator, Str.Str, List);
    va_end(List);
    return Result;
}

const uint16_t& str16::operator[](size_t Index) const
{
    Assert(Index < Length);
    return Str[Index];
}

size_t Str16_Length(const uint16_t* Str)
{
    size_t Result = 0;
    while(*Str++) Result++;
    return Result;
}

str16 Str16(const uint16_t* Str, size_t Length)
{
    str16 Result = {};
    Result.Str = Str;
    Result.Length = Length;
    return Result;
}

str16 Str16(const uint16_t* Str)
{
    return Str16(Str, Str16_Length(Str));
}

str16 UTF8_To_UTF16(allocator* Allocator, const str8& Str)
{
    utf16_stream_writer Writer = UTF16_Stream_Writer(Allocator, Str.Length+1);
    utf8_stream_reader Reader = UTF8_Stream_Reader(Str);
    
    while(Reader.Is_Valid())
    {
        uint32_t Codepoint = Reader.Consume();
        Writer.Write(Codepoint);
    }
    
    str16 Result = Writer.To_Str16();
    return Result;
}
