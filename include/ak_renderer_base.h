#ifndef AK_RENDERER_BASE_H
#define AK_RENDERER_BASE_H

#ifdef _MSC_VER
#include <intrin.h>
#define Write_Barrier() _WriteBarrier()
#define Debug_Break() __debugbreak()
#define thread_var __declspec(thread)
#endif

#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))
#define Abs(x) ((x) < 0 ? -(x) : (x))
#define Clamp(Min, X, Max) ((V < Min) ? Min : ((V > Max) ? Max : V))
#define Saturate(V) Clamp(0.0f, V, 1.0f)
#define Lerp(A, t, B) ((A) + (t)*((B)-(A)))

#define Assert(condition) do { if(!(condition)) Debug_Break(); } while(0)

#define Kilobyte(byte) ((byte)*1024LL)
#define Megabyte(byte) (Kilobyte(byte)*1024LL)
#define Gigabyte(byte) (Megabyte(byte)*1024LL)
#define Terabyte(byte) (Gigabyte(byte)*1024LL)

#include "util/ak_renderer_util.h"

#ifdef _WIN32
#include "os/win32/win32_renderer.h"
#endif

#define THREAD_CALLBACK(name) int name(void* UserData)
typedef THREAD_CALLBACK(thread_callback);

thread Create_Thread(thread_callback* ThreadCallback, void* UserData);
void Wait_Thread(thread Thread);
void Delete_Thread(thread Thread);

semaphore Create_Semaphore(uint32_t InitialCount);
void Decrement_Semaphore(semaphore Semaphore);
void Increment_Semaphore(semaphore Semaphore);
void Delete_Semaphore(semaphore Semaphore);

event Create_Event();
void Wait_Event(event Event);
void Signal_Event(event Event);
void Delete_Event(event Event);

static void* Memory_Allocate(size_t Size);
static void  Memory_Free(void* Memory);
static void* Memory_Allocate_Aligned(size_t Size, size_t Alignment);
static void  Memory_Free_Aligned(void* Memory);

#endif