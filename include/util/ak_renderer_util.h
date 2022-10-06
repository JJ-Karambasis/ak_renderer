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

#include "ak_renderer_random.h"
#include "ak_renderer_atomic.h"
#include "ak_renderer_memory.h"
#include "ak_renderer_string.h"

struct thread_context
{
    async_arena* MainArena;
    arena*       ScratchArena;
};

void Set_Thread_Context(thread_context* ThreadContext);
thread_context* Get_Thread_Context();

#endif