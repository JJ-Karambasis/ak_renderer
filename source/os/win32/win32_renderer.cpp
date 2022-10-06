static ak_renderer_init_result Win32_Init_Renderer()
{
    return VK_Init_Renderer();
}

#define AK__WIN32_SEMAPHORE_MAX_VALUE 512

struct win32_thread_callback_data
{
    thread_callback* Callback;
    void* UserData;
    HANDLE CreationEvent;
};

DWORD Win32_Thread_Proc(LPVOID Parameter)
{
    win32_thread_callback_data* Data = (win32_thread_callback_data*)Parameter;
    thread_callback* Callback = Data->Callback;
    void* UserData = Data->UserData;
    Write_Barrier();
    SetEvent(Data->CreationEvent);
    return Callback(UserData);
}

thread Create_Thread(thread_callback* ThreadCallback, void* UserData)
{
    win32_thread_callback_data ThreadCallbackData = {};
    ThreadCallbackData.Callback = ThreadCallback;
    ThreadCallbackData.UserData = UserData;
    ThreadCallbackData.CreationEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    thread Result = CreateThread(NULL, 0, Win32_Thread_Proc, &ThreadCallbackData, 0, NULL);
    WaitForSingleObject(ThreadCallbackData.CreationEvent, INFINITE);
    CloseHandle(ThreadCallbackData.CreationEvent);
    return Result;
}

inline void Wait_Thread(thread Thread)
{
    WaitForSingleObject(Thread, INFINITE);
}

inline void Delete_Thread(thread Thread)
{
    CloseHandle(Thread);
}

inline semaphore Create_Semaphore(uint32_t InitialCount)
{
    semaphore Semaphore = CreateSemaphoreA(NULL, InitialCount, AK__WIN32_SEMAPHORE_MAX_VALUE, NULL);
    return Semaphore;
}

inline void Decrement_Semaphore(semaphore Semaphore)
{
    WaitForSingleObject(Semaphore, INFINITE);
}

inline void Increment_Semaphore(semaphore Semaphore)
{
    ReleaseSemaphore(Semaphore, 1, NULL);
}

inline void Delete_Semaphore(semaphore Semaphore)
{
    CloseHandle(Semaphore);
}

event Create_Event()
{
    event Event = CreateEventA(NULL, FALSE, FALSE, NULL);
    return Event;
}

void Wait_Event(event Event)
{
    WaitForSingleObject(Event, INFINITE);
}

void Signal_Event(event Event)
{
    SetEvent(Event);
}

void Delete_Event(event Event)
{
    CloseHandle(Event);
}

#include "../../api/vk/vk_renderer.cpp"