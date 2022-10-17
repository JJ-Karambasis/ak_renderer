#ifndef AK_RENDERER_MATERIAL_PARSER_H
#define AK_RENDERER_MATERIAL_PARSER_H

struct material_parser
{
    bool IsSuccessful;
};

material_parser Parse_Material(arena* Arena, const char* Source, size_t SourceSize);

#endif