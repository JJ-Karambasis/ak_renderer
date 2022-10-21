void ak_cmd_buffer_impl::Reset()
{
    TempCommandArena.End();
    TempCommandArena.Begin(Arena);
    CmdArena = Create_Arena(Arena, Kilobyte(100));
    Cmds = Create_Bucket_Array<ak_cmd*>(CmdArena, 128);
}

ak_cmd_lighting_pass* ak_cmd_buffer_impl::Begin_Lighting_Pass(const ak_lighting_pass_begin_info* LightingPassBeginInfo)
{
    ak_cmd_begin_lighting_pass* Cmd = CmdArena->Push_Struct<ak_cmd_begin_lighting_pass>();
    Cmd->Type = AK_CMD_TYPE_BEGIN_LIGHTING_PASS;
    Cmds.Add(Cmd);
    
    ak_cmd_lighting_pass_impl* LightingPass = &Cmd->LightingPass;
    LightingPass->CmdArena = Create_Arena(CmdArena, Kilobyte(100));
    LightingPass->Cmds = Create_Bucket_Array<ak_lighting_cmd*>(LightingPass->CmdArena, 128);
    
    return LightingPass;
}

ak_cmd_buffer_impl* Allocate_Command_Buffer()
{
    thread_context* ThreadContext = Get_Thread_Context();
    arena* Arena = Create_Arena(ThreadContext->MainArena, Megabyte(1));
    ak_cmd_buffer_impl* Result = new(Arena) ak_cmd_buffer_impl;
    Memory_Clear(Result, sizeof(ak_cmd_buffer_impl));
    Result->Arena = Arena;
    return Result;
}