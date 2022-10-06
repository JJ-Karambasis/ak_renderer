#ifndef AK_RENDERER_BASE_H
#define AK_RENDERER_BASE_H

#include <util/ak_renderer_memory.h>
#include <util/ak_renderer_string.h>

#ifdef _WIN32
#include <os/win32/win32_renderer.h>
#endif

static void* Memory_Allocate(size_t Size);
static void  Memory_Free(void* Memory);
static void* Memory_Allocate_Aligned(size_t Size, size_t Alignment);
static void  Memory_Free_Aligned(void* Memory);

#endif
