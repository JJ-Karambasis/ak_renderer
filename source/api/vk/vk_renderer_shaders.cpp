static const char G_ShaderCommonVK[] = u8R"(
                                   #version 450
                                   
                                   typedef vec3 v3;
                                   typedef mat4x3 m4x3;
                                   
                                   )";

static const char G_ModelVertexShaderVK[] = R"(
                                        
                                        layout(location = 0) in v3 InPosition;
                                        layout(location = 1) in v3 InNormal;
                                        
                                        void main()
                                        {
                                        gl_Position = V4(InPosition, 1.0f);
                                        }
                                        
                                        )";

static const char G_LambertianFragmentShaderVK[] = R"(
                                               
                                               layout(location = 0) out v4 OutColor;
                                               
                                               void main()
                                               {
                                               material_eval MaterialEval = Evaluate();
                                               OutColor = V4(MaterialEval.Diffuse, 1.0f);
                                               }
                                               
                                               )";

bool VK_Init_Glslang()
{
    return glslang_initialize_process();
}

str8 VK_Convert_To_Glsl(const material_parser* Parser, arena* Arena)
{
    return {};
}

void Delete_Ptr(glslang_shader_t* Shader, void* UserData)
{
    glslang_shader_delete(Shader);
}

void Delete_Ptr(glslang_program_t* Program, void* UserData)
{
    glslang_program_delete(Program);
}

array<uint32_t> VK_Compile_Shader(arena* Arena, const str8& Source, glslang_stage_t Stage)
{
    glslang_input_t Input = {};
    Input.language = GLSLANG_SOURCE_GLSL;
    Input.stage = Stage;
    Input.client = GLSLANG_CLIENT_VULKAN;
    Input.client_version = GLSLANG_TARGET_VULKAN_1_0;
    Input.target_language = GLSLANG_TARGET_SPV;
    Input.target_language_version = GLSLANG_TARGET_SPV_1_0,
    Input.code = (const char*)Source.Str;
    Input.default_version = 100;
    Input.default_profile = GLSLANG_NO_PROFILE;
    Input.force_default_version_and_profile = false;
    Input.forward_compatible = false;
    Input.messages = GLSLANG_MSG_DEFAULT_BIT;
    Input.resource = (const glslang_resource_t*)(&glslang::DefaultTBuiltInResource);
    
    auto_ptr<glslang_shader_t> Shader(glslang_shader_create(&Input));
    Bool_Warning_Check(Shader, Str8_Lit("Failed to create the vulkan glsl shader"));
    Bool_Warning_Check(glslang_shader_preprocess(Shader, &Input), Str8_Lit("GLSL preprocessing failed\nInfo Message: %s\nDebug Message: %s"), 
                       glslang_shader_get_info_log(Shader), glslang_shader_get_info_debug_log(Shader));
    Bool_Warning_Check(glslang_shader_parse(Shader, &Input), 
                       Str8_Lit("GLSL parsing failed\nInfo Message: %s\nDebug Message: %s"), 
                       glslang_shader_get_info_log(Shader), glslang_shader_get_info_debug_log(Shader));
    
    auto_ptr<glslang_program_t> Program(glslang_program_create());
    Bool_Warning_Check(Program, Str8_Lit("Failed to create the vulkan glsl program"));
    
    glslang_program_add_shader(Program, Shader);
    
    Bool_Warning_Check(glslang_program_link(Program, GLSLANG_MSG_SPV_RULES_BIT|GLSLANG_MSG_VULKAN_RULES_BIT), 
                       Str8_Lit("GLSL linking failed\nInfo Message: %s\nDebug Message: %s"), glslang_program_get_info_log(Program), glslang_program_get_info_debug_log(Program));
    
    glslang_program_SPIRV_generate(Program, Stage);
    
    array<uint32_t> Result = Allocate_Array<uint32_t>(Arena, glslang_program_SPIRV_get_size(Program));
    glslang_program_SPIRV_get(Program, Result.Data);
    
    return Result;
}