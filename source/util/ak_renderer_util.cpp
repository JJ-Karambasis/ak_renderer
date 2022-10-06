#include "util/ak_renderer_random.cpp"
#include "util/ak_renderer_atomic.cpp"
#include "util/ak_renderer_memory.cpp"
#include "util/ak_renderer_string.cpp"

static thread_var thread_context* GThreadContext;

void Set_Thread_Context(thread_context* ThreadContext)
{
    GThreadContext = ThreadContext;
}

thread_context* Get_Thread_Context()
{
    return GThreadContext;
}