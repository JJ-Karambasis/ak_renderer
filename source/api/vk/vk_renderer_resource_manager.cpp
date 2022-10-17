ak_handle<ak_material> vk_resource_manager::Create_Material(const char* Source, size_t SourceSize)
{
    arena* Scratch = Get_Thread_Context()->ScratchArena;
    
    material_parser Parser = Parse_Material(Scratch, Source, SourceSize);
    Bool_Warning_Check(Parser.IsSuccessful, Str8_Lit("Failed to parse the material!"));
    
    str8 Common = Str8_Expand(G_ShaderCommonVK);
    str8 VertexShaderTemplate = Str8_Expand(G_ModelVertexShaderVK);
    str8 MaterialEval = VK_Convert_To_Glsl(&Parser, Scratch);
    str8 FragmentShaderTemplate = Str8_Expand(G_LambertianFragmentShaderVK);
    
    str8_list VertexShaderList = {};
    VertexShaderList.Push(Scratch, Common);
    VertexShaderList.Push(Scratch, VertexShaderTemplate);
    
    str8_list FragmentShaderList = {};
    FragmentShaderList.Push(Scratch, Common);
    FragmentShaderList.Push(Scratch, MaterialEval);
    FragmentShaderList.Push(Scratch, FragmentShaderTemplate);
    
    array<uint32_t> VertexShader = VK_Compile_Shader(Scratch, VertexShaderList.Join(Scratch), 
                                                     GLSLANG_STAGE_VERTEX);
    array<uint32_t> FragmentShader = VK_Compile_Shader(Scratch, FragmentShaderList.Join(Scratch), 
                                                       GLSLANG_STAGE_FRAGMENT);
    
    Bool_Warning_Check(!VertexShader.Is_Empty(), Str8_Lit("Failed to compile the material vertex shader"));
    Bool_Warning_Check(FragmentShader.Is_Empty(), Str8_Lit("Failed to compile the material fragment shader"));
    
    return {};
}

bool VK_Init_Resource_Manager(vk_device_context* DeviceContext)
{
    vk_resource_manager* ResourceManager = &DeviceContext->ResourceManager;
    ResourceManager->DeviceContext = DeviceContext;
    ResourceManager->Materials = Create_Async_Pool<vk_material>(Get_Thread_Context()->MainArena, 64);
    return true;
}