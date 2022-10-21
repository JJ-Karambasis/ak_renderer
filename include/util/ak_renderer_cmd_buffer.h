#ifndef AK_RENDERER_CMD_BUFFER_H
#define AK_RENDERER_CMD_BUFFER_H

enum ak_lighting_cmd_type
{
    AK_LIGHTING_CMD_TYPE_UNKNOWN,
    AK_LIGHTING_CMD_TYPE_DRAW
};

struct ak_lighting_cmd
{
    ak_lighting_cmd_type Type;
};

struct ak_lighting_cmd_draw : public ak_lighting_cmd
{
    ak_handle<ak_mesh> Mesh;
    uint32_t IndexCount;
    uint32_t IndexOffset;
    uint32_t VertexOffset;
    float    Transform[12];
    float    Color[3];
};

struct ak_cmd_lighting_pass_impl : public ak_cmd_lighting_pass
{
    arena* CmdArena;
    bucket_array<ak_lighting_cmd*> Cmds;
    
    ak_lighting_pass_begin_info Info;
    void Draw(ak_handle<ak_mesh> Mesh, uint32_t IndexCount, uint32_t IndexOffset, uint32_t VertexOffset, float* Transform, float* Color) final;
};

enum ak_cmd_type
{
    AK_CMD_TYPE_UNKNOWN,
    AK_CMD_TYPE_BEGIN_LIGHTING_PASS
};

struct ak_cmd
{
    ak_cmd_type Type;
};

struct ak_cmd_begin_lighting_pass : public ak_cmd
{
    ak_cmd_lighting_pass_impl LightingPass;
};

struct ak_cmd_buffer_impl : public ak_cmd_buffer
{
    arena* Arena;
    temp_arena TempCommandArena;
    
    arena* CmdArena;
    bucket_array<ak_cmd*> Cmds;
    
    void Reset() final;
    ak_cmd_lighting_pass* Begin_Lighting_Pass(const ak_lighting_pass_begin_info* LightingPassBeginInfo) final;
};

ak_cmd_buffer_impl* Allocate_Command_Buffer();

#endif