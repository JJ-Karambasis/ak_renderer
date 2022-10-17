#ifdef _WIN32
#include <windows.h>

struct window
{
    HWND Handle;
    ak_handle<ak_display> Display;
};

static LRESULT Win32__Window_Proc(HWND Window, UINT Msg, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    
    switch(Msg)
    {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;
        
        default:
        {
            Result = DefWindowProcA(Window, Msg, WParam, LParam);
        } break;
    }
    
    return Result;
}

static bool G__InitWindowClass; 
window Create_Window(ak_display_manager* DisplayManager, uint32_t Width, uint32_t Height)
{
    if(!G__InitWindowClass)
    {
        WNDCLASSA WindowClass = {};
        WindowClass.style = CS_OWNDC;
        WindowClass.hInstance = GetModuleHandle(0);
        WindowClass.lpfnWndProc = DefWindowProcA;
        WindowClass.lpszClassName = "Test_App";
        
        if(!RegisterClass(&WindowClass)) return {};
        
        G__InitWindowClass = true;
    }
    
    window Window = {};
    Window.Handle = CreateWindowA("Test_App", "Test_App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, GetModuleHandle(0), NULL);
    if(!Window.Handle) return {};
    
    Window.Display = DisplayManager->Create_Display(Window.Handle);
    if(Window.Display.Is_Null()) return {};
    
    ShowWindow(Window.Handle, SW_SHOW);
    
    return Window;
}

bool Poll_OS()
{
    MSG Message;
    while(PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT:
            {
                return false;
            } break;
            
            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            } break;
        }
    }
    
    return true;
}

#endif