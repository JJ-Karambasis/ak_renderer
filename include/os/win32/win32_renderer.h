#ifndef WIN32_RENDERER_H
#define WIN32_RENDERER_H

#include "../../api/vk/vk_renderer.h"

typedef HANDLE thread;
typedef HANDLE semaphore;
typedef HANDLE event;

static ak_renderer_init_result Win32_Init_Renderer();
static ak_device_context* Win32_Set_Device(ak_device* Device);

#endif