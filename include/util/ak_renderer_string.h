#ifndef AK_RENDERER_STRING_H
#define AK_RENDERER_STRING_H

#include <stb/stb_sprintf.h>

enum string_comparison_method
{
    STRING_COMPARISION_METHOD_CASE_SENSITIVE,
    STRING_COMPARISION_METHOD_CASE_INSENSITIVE
};

#define Str8_Lit(s) Str8((const uint8_t*)(u8##s), sizeof((u8##s))-1)
#define Str8_Expand(s) {(const uint8_t*)s, sizeof(s)-1}
struct str8
{
    const uint8_t* Str;
    size_t         Length;
    const uint8_t& operator[](size_t Index) const;
};

size_t  Str8_Length(const uint8_t* Str);
str8   Str8(const uint8_t* Str, size_t Length);
str8   Str8(const uint8_t* Str);
str8   Str8_FormatV(allocator* Allocator, const uint8_t* Format, va_list Args);
str8   Str8_Format(allocator* Allocator, const uint8_t* Format, ...);
str8   Str8_FormatV(allocator* Allocator, const str8& Str, va_list Args);
str8   Str8_Format(allocator* Allocator, const str8& Str, ...);
bool   Str8_Equal(const str8& StrA, const str8& StrB, string_comparison_method Method = STRING_COMPARISION_METHOD_CASE_SENSITIVE);
bool   operator==(const str8& StrA, const str8& StrB);

uint32_t Hash_Function(const str8& Str);

struct str8_node
{
    str8       String;
    str8_node* Next;
};

struct str8_list
{
    str8_node* First;
    str8_node* Last;
    size_t      TotalLength;
    uint32_t    NodeCount;
    
    void Push(str8_node* Node);
    void Push(allocator* Allocator, const str8& Str);
    void FormatV(allocator* Allocator, const uint8_t* Format, va_list Args);
    void Format(allocator* Allocator, const uint8_t* Format, ...);
    void FormatV(allocator* Allocator, const str8& Format, va_list Args);
    void Format(allocator* Allocator, const str8& Format, ...);
    
    str8 Join(allocator* Allocator) const;
};

//NOTE(EVERYONE): UTF-16 string
#define Str16_Lit(s) Str16((const uint16_t*)(u##s), (sizeof((u##s))/2) -1)
struct str16
{
    const uint16_t* Str;
    size_t       Length;
    const uint16_t& operator[](size_t Index) const;
};

size_t  Str16_Length(const uint16_t* Str);
str16  Str16(const uint16_t* Str, size_t Length);
str16  Str16(const uint16_t* Str);

str16 UTF8_To_UTF16(allocator* Allocator, const str8& Str);

struct logger
{
    ak_renderer_log_callback* LogDebug;
    ak_renderer_log_callback* LogInfo;
    ak_renderer_log_callback* LogWarning;
    ak_renderer_log_callback* LogError;
    void*                     UserData;
};

static logger GLogger;

#endif