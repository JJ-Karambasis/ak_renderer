#include "util/ak_renderer_random.cpp"
#include "util/ak_renderer_atomic.cpp"
#include "util/ak_renderer_memory.cpp"
#include "util/ak_renderer_string.cpp"
#include "util/ak_renderer_array.cpp"
#include "util/ak_renderer_hash.cpp"
#include "util/ak_renderer_async_pool.cpp"
#include "util/ak_renderer_cmd_buffer.cpp"
#include "ak_renderer_material_parser.cpp"

static thread_var thread_context* GThreadContext;

thread_context* Create_Thread_Context(async_arena* MainArena)
{
    thread_context* ThreadContext = MainArena->Push_Struct<thread_context>();
    ThreadContext->MainArena = MainArena;
    ThreadContext->ScratchArena = Create_Arena(&GAllocator, Megabyte(32));
    Bool_Warning_Check(ThreadContext->ScratchArena, Str8_Lit("Failed to create the scratch arena"));
    Set_Thread_Context(ThreadContext);
    return ThreadContext;
}

void Set_Thread_Context(thread_context* ThreadContext)
{
    GThreadContext = ThreadContext;
}

thread_context* Get_Thread_Context()
{
    return GThreadContext;
}

uint32_t Ceil_Pow2(uint32_t V)
{
    --V;
    V |= V >> 1;
    V |= V >> 2;
    V |= V >> 4;
    V |= V >> 8;
    V |= V >> 16;
    ++V;
    V += ( V == 0 );    
    return V;
}

uint8_t To_Upper(uint8_t C)
{
    if (('a' <= C) && (C <= 'z'))
        C -= 'a' - 'A';
    return C;
}

inline bool Equal_Zero_Approx(float V,  float Eps)
{
    return Abs(V) < Eps;
}

inline bool Equal_Zero_Approx(double V, double Eps)
{
    return Abs(V) < Eps;
}

inline bool Equal_Approx(float A, float B, float Eps)
{
    return Equal_Zero_Approx(A-B, Eps);
}

inline bool Equal_Approx(double A, double B, double Eps)
{
    return Equal_Zero_Approx(A-B, Eps);
}

inline bool Equal_Zero_Eps(float V)
{
    return Equal_Zero_Approx(V, EpsilonF32);
}

inline bool Equal_Zero_Eps(double V)
{
    return Equal_Zero_Approx(V, EpsilonF64);
}

inline bool Equal_Eps(float A, float B)
{
    return Equal_Approx(A, B, EpsilonF32);
}

inline bool Equal_Eps(double A, double B)
{
    return Equal_Approx(A, B, EpsilonF64);
}

inline float Safe_Ratio(float A, float B)
{
    Assert(!Equal_Zero_Eps(B));
    return A/B;
}

inline double Safe_Ratio(double A, double B)
{
    Assert(!Equal_Zero_Eps(B));
    return A/B;
}

inline float Safe_RatioF32(int32_t A, int32_t B)
{
    Assert(B);
    return (float)A/(float)B;
}

inline double Safe_RatioF64(int32_t A, int32_t B)
{
    Assert(B);
    return (double)A/(double)B;
}

inline uint8_t SafeU8(uint16_t V)
{
    Assert(V <= MaxU8);
    return (uint8_t)V;
}

inline uint8_t SafeU8(uint32_t V)
{
    Assert(V <= MaxU8);
    return (uint8_t)V;
}

inline uint8_t SafeU8(uint64_t V)
{
    Assert(V <= MaxU8);
    return (uint8_t)V;
}

inline uint8_t SafeU8(int8_t V)
{
    Assert(V >= 0);
    return (uint8_t)V;
}

inline uint8_t SafeU8(int16_t V)
{
    Assert(V >= 0 && V <= MaxU8);
    return (uint8_t)V;
}

inline uint8_t SafeU8(int32_t V)
{
    Assert(V >= 0 && V <= MaxU8);
    return (uint8_t)V;
}

inline uint8_t SafeU8(int64_t V)
{
    Assert(V >= 0 && V <= MaxU8);
    return (uint8_t)V;
}

inline uint8_t SafeU8(float V)
{
    Assert(V >= 0 && V <= (float)MaxU8);
    return (uint8_t)V;
}

inline uint8_t SafeU8(double V)
{
    Assert(V >= 0 && V <= (double)MaxU8);
    return (uint8_t)V;
}

inline uint16_t SafeU16(uint32_t V)
{
    Assert(V <= MaxU16);
    return (uint16_t)V;
}

inline uint16_t SafeU16(uint64_t V)
{
    Assert(V <= MaxU16);
    return (uint16_t)V;
}

inline uint16_t SafeU16(int8_t V)
{
    Assert(V >= 0);
    return (uint16_t)V;
}

inline uint16_t SafeU16(int16_t V)
{
    Assert(V >= 0);
    return (uint16_t)V;
}

inline uint16_t SafeU16(int32_t V)
{
    Assert(V >= 0 && V < MaxU16);
    return (uint16_t)V;
}

inline uint16_t SafeU16(int64_t V)
{
    Assert(V >= 0 && V < MaxU16);
    return (uint16_t)V;
}

inline uint16_t SafeU16(float V)
{
    Assert(V >= 0 && V <= (float)MaxU16);
    return (uint8_t)V;
}

inline uint16_t SafeU16(double V)
{
    Assert(V >= 0 && V <= (double)MaxU16);
    return (uint8_t)V;
}

inline uint32_t SafeU32(uint64_t V)
{
    Assert(V <= MaxU32);
    return (uint32_t)V;
}

inline uint32_t SafeU32(int8_t V)
{
    Assert(V >= 0);
    return (uint32_t)V;
}

inline uint32_t SafeU32(int16_t V)
{
    Assert(V >= 0);
    return (uint32_t)V;
}

inline uint32_t SafeU32(int32_t V)
{
    Assert(V >= 0);
    return (uint32_t)V;
}

inline uint32_t SafeU32(int64_t V)
{
    Assert(V >= 0 && V <= MaxU32);
    return (uint32_t)V;
}

inline uint32_t SafeU32(float V)
{
    Assert(V >= V && V <= (float)MaxU32);
    return (uint32_t)V;
}

inline uint32_t SafeU32(double V)
{
    Assert(V >= V && V <= (double)MaxU32);
    return (uint32_t)V;
}

inline uint64_t SafeU64(int8_t V)
{
    Assert(V >= 0);
    return (uint64_t)V;
}

inline uint64_t SafeU64(int16_t V)
{
    Assert(V >= 0);
    return (uint64_t)V;
}

inline uint64_t SafeU64(int32_t V)
{
    Assert(V >= 0);
    return (uint64_t)V;
}

inline uint64_t SafeU64(int64_t V)
{
    Assert(V >= 0);
    return (uint64_t)V;
}

inline uint64_t SafeU64(float V)
{
    Assert(V >= 0);
    return (uint64_t)V;
}

inline uint64_t SafeU64(double V)
{
    Assert(V >= 0);
    return (uint64_t)V;
}

inline int8_t SafeS8(uint8_t V)
{
    Assert(V <= MaxS8);
    return (int8_t)V;
}

inline int8_t SafeS8(uint16_t V)
{
    Assert(V <= MaxS8);
    return (int8_t)V;
}

inline int8_t SafeS8(uint32_t V)
{
    Assert(V <= MaxS8);
    return (int8_t)V;
}

inline int8_t SafeS8(uint64_t V)
{
    Assert(V <= MaxS8);
    return (int8_t)V;
}

inline int8_t SafeS8(int16_t V)
{
    Assert(V >= MinS8 && V <= MaxS8);
    return (int8_t)V;
}

inline int8_t SafeS8(int32_t V)
{
    Assert(V >= MinS8 && V <= MaxS8);
    return (int8_t)V;
}

inline int8_t SafeS8(int64_t V)
{
    Assert(V >= MinS8 && V <= MaxS8);
    return (int8_t)V;
}

inline int16_t SafeS16(uint16_t V)
{
    Assert(V <= MaxS16);
    return (int16_t)V;
}

inline int16_t SafeS16(uint32_t V)
{
    Assert(V <= MaxS16);
    return (int16_t)V;
}

inline int16_t SafeS16(uint64_t V)
{
    Assert(V <= MaxS16);
    return (int16_t)V;
}

inline int16_t SafeS16(int32_t V)
{
    Assert(V >= MinS16 && V <= MaxS16);
    return (int16_t)V;
}

inline int16_t SafeS16(int64_t V)
{
    Assert(V >= MinS16 && V <= MaxS16);
    return (int16_t)V;
}

inline int32_t SafeS32(uint32_t V)
{
    Assert(V <= MaxS32);
    return (int32_t)V;
}

inline int32_t SafeS32(uint64_t V)
{
    Assert(V <= MaxS32);
    return (int32_t)V;
}

inline int32_t SafeS32(int64_t V)
{
    Assert(V >= MinS32 && V <= MaxS32);
    return (int32_t)V;
}

inline int64_t SafeS64(uint64_t V)
{
    Assert(V <= MaxS64);
    return (uint64_t)V;
}