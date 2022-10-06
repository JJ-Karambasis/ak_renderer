#ifndef AK_RENDERER_STRING_H
#define AK_RENDERER_STRING_H

struct logger
{
    ak_renderer_log_callback* LogDebug;
    ak_renderer_log_callback* LogInfo;
    ak_renderer_log_callback* LogWarning;
    ak_renderer_log_callback* LogError;
    void*                     UserData;
};

static logger GLogger;

#endif