#ifndef AK_RENDERER_STRING_H
#define AK_RENDERER_STRING_H

#include <stb/stb_sprintf.h>

#define Str8_Lit(s) Str8((const uint8_t*)(u8##s), sizeof((u8##s))-1)
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