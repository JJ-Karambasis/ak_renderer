#ifndef AK_RENDERER_UTIL_H
#define AK_RENDERER_UTIL_H

#define SLL_Stack_Push_NP(head, entry, next) (entry)->next = (head); (head) = entry
#define SLL_Stack_Pop_NP(head, next) (head)=(head)->next
#define SLL_Queue_Push_NP(first, last, entry, next) ((first) ? (last)->next = (entry) : (first) = (last) = (entry))
#define SLL_Queue_Pop_NP(first, last, next) (((first) == (last)) ? (first) = (last) = NULL : (first) = (first)->next)

#define SLL_Stack_Push(head, entry) SLL_Stack_Push_NP(head, entry, Next)
#define SLL_Stack_Pop(head) SLL_Stack_Pop_NP(head, Next)
#define SLL_Queue_Push(first, last, entry) SLL_Queue_Push_NP(first, last, entry, Next)
#define SLL_Queue_Pop(first, last) SLL_Queue_Pop_NP(first, last, Next)

static const uint32_t Bitmask_1  = 0x00000001;
static const uint32_t Bitmask_2  = 0x00000003;
static const uint32_t Bitmask_3  = 0x00000007;
static const uint32_t Bitmask_4  = 0x0000000f;
static const uint32_t Bitmask_5  = 0x0000001f;
static const uint32_t Bitmask_6  = 0x0000003f;
static const uint32_t Bitmask_7  = 0x0000007f;
static const uint32_t Bitmask_8  = 0x000000ff;
static const uint32_t Bitmask_9  = 0x000001ff;
static const uint32_t Bitmask_10 = 0x000003ff;
static const uint32_t Bitmask_11 = 0x000007ff;
static const uint32_t Bitmask_12 = 0x00000fff;
static const uint32_t Bitmask_13 = 0x00001fff;
static const uint32_t Bitmask_14 = 0x00003fff;
static const uint32_t Bitmask_15 = 0x00007fff;
static const uint32_t Bitmask_16 = 0x0000ffff;
static const uint32_t Bitmask_17 = 0x0001ffff;
static const uint32_t Bitmask_18 = 0x0003ffff;
static const uint32_t Bitmask_19 = 0x0007ffff;
static const uint32_t Bitmask_20 = 0x000fffff;
static const uint32_t Bitmask_21 = 0x001fffff;
static const uint32_t Bitmask_22 = 0x003fffff;
static const uint32_t Bitmask_23 = 0x007fffff;
static const uint32_t Bitmask_24 = 0x00ffffff;
static const uint32_t Bitmask_25 = 0x01ffffff;
static const uint32_t Bitmask_26 = 0x03ffffff;
static const uint32_t Bitmask_27 = 0x07ffffff;
static const uint32_t Bitmask_28 = 0x0fffffff;
static const uint32_t Bitmask_29 = 0x1fffffff;
static const uint32_t Bitmask_30 = 0x3fffffff;
static const uint32_t Bitmask_31 = 0x7fffffff;

static const uint8_t  MaxU8 = 0xFF;
static const uint16_t MaxU16 = 0xFFFF;
static const uint32_t MaxU32 = 0xFFFFFFFF;
static const uint64_t MAxU64 = 0xFFFFFFFFFFFFFFFF;

static const int8_t  MaxS8 = 127;
static const int16_t MaxS16 = 32767;
static const int32_t MaxS32 = 2147483647;
static const int64_t MaxS64 = 9223372036854775807;

static const int8_t  MinS8   = -127 - 1;
static const int16_t MinS16 = -32767 - 1;
static const int32_t MinS32 = -2147483647 - 1;
static const int64_t MinS64 = -9223372036854775807 - 1;

static const float MaxF32 = 3.402823466e+38f;
static const float MinF32 = -MaxF32;
static const float Smallest_PositiveF32 = 1.1754943508e-38f;
static const float EpsilonF32 = 5.96046448e-8f;
static const float PiF32 = 3.14159265359f;
static const float TauF32 = 6.28318530718f;
static const float ExpF32 = 2.71828182846f;
static const float InfinityF32 = (float)0x7F800000;

static const double MaxF64 = 1.79769313486231e+308;
static const double MinF64 = -MaxF64;
static const double Smallest_PositiveF64 = 4.94065645841247e-324;
static const double EpsilonF64 = 1.11022302462515650e-16;
static const double PiF64 = 3.14159265359;
static const double TauF64 = 6.28318530718;
static const double ExpF64 = 2.71828182846;
static const double InfinityF64 = (float)0x7FF0000000000000;

#include "ak_renderer_random.h"
#include "ak_renderer_atomic.h"
#include "ak_renderer_memory.h"
#include "ak_renderer_string.h"
#include "ak_renderer_array.h"
#include "ak_renderer_hash.h"

struct thread_context
{
    async_arena* MainArena;
    arena*       ScratchArena;
};

thread_context* Create_Thread_Context(async_arena* MainArena);
void Set_Thread_Context(thread_context* ThreadContext);
thread_context* Get_Thread_Context();
uint32_t Ceil_Pow2(uint32_t V);
uint8_t To_Upper(uint8_t C);
bool Equal_Zero_Approx(float V,  float Eps);
bool Equal_Zero_Approx(double V, double Eps);
bool Equal_Approx(float A, float B, float Eps);
bool Equal_Approx(double A, double B, double Eps);
bool Equal_Zero_Eps(float V);
bool Equal_Zero_Eps(double V);
bool Equal_Eps(float A, float B);
bool Equal_Eps(double A, double B);
float Safe_Ratio(float A, float B);
double Safe_Ratio(double A, double B);
float Safe_RatioF32(int32_t A, int32_t B);
double Safe_RatioF64(int32_t A, int32_t B);
uint8_t SafeU8(uint16_t V);
uint8_t SafeU8(uint32_t V);
uint8_t SafeU8(uint64_t V);
uint8_t SafeU8(int8_t V);
uint8_t SafeU8(int16_t V);
uint8_t SafeU8(int32_t V);
uint8_t SafeU8(int64_t V);
uint8_t SafeU8(float V);
uint8_t SafeU8(double V);
uint16_t SafeU16(uint32_t V);
uint16_t SafeU16(uint64_t V);
uint16_t SafeU16(int8_t V);
uint16_t SafeU16(int16_t V);
uint16_t SafeU16(int32_t V);
uint16_t SafeU16(int64_t V);
uint16_t SafeU16(float V);
uint16_t SafeU16(double V);
uint32_t SafeU32(uint64_t V);
uint32_t SafeU32(int8_t V);
uint32_t SafeU32(int16_t V);
uint32_t SafeU32(int32_t V);
uint32_t SafeU32(int64_t V);
uint32_t SafeU32(float V);
uint32_t SafeU32(double V);
uint64_t SafeU64(int8_t V);
uint64_t SafeU64(int16_t V);
uint64_t SafeU64(int32_t V);
uint64_t SafeU64(int64_t V);
uint64_t SafeU64(float V);
uint64_t SafeU64(double V);
int8_t SafeS8(uint8_t V);
int8_t SafeS8(uint16_t V);
int8_t SafeS8(uint32_t V);
int8_t SafeS8(uint64_t V);
int8_t SafeS8(int16_t V);
int8_t SafeS8(int32_t V);
int8_t SafeS8(int64_t V);
int16_t SafeS16(uint16_t V);
int16_t SafeS16(uint32_t V);
int16_t SafeS16(uint64_t V);
int16_t SafeS16(int32_t V);
int16_t SafeS16(int64_t V);
int32_t SafeS32(uint32_t V);
int32_t SafeS32(uint64_t V);
int32_t SafeS32(int64_t V);
int64_t SafeS64(uint64_t V);

#endif