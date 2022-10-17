static const char G_TestMaterial0[] = R"(

material_inputs
{
color3f Color;
};
                                  
                                  material_eval Evaluate()
                                  {
                                  material_eval Result = (material_eval)0;
                                  Result.Diffuse = Color;
                                  return Result;
                                  }
                                  
                                  )";

static const char G_TestMaterial1[] = R"(

material_inputs
{
texture2D Texture;
sampler Sampler;
};

material_eval Evaluate()
{
material_eval Result = (material_eval)0;
                                  Result.Diffuse = Color;
                                  return Result;
}

)";