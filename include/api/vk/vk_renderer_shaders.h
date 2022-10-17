#ifndef VK_RENDERER_SHADERS_H
#define VK_RENDERER_SHADERS_H

#include <glslang/glslang/Include/glslang_c_interface.h>
#include <glslang/StandAlone/ResourceLimits.h>

bool VK_Init_Glslang();
str8 VK_Convert_To_Glsl(const material_parser* Parser, arena* Arena);
array<uint32_t> VK_Compile_Shader(arena* Arena, const str8& Source, glslang_stage_t Stage);

#endif