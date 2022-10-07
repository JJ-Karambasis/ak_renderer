static HMODULE GLibrary;

static bool VK_Load_Library()
{
    GLibrary = LoadLibrary("vulkan-1.dll");
    Bool_Warning_Check(GLibrary, Str8_Lit("Failed to load vulkan-1.dll"));
    
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(GLibrary, "vkGetInstanceProcAddr");
    Bool_Warning_Check(vkGetInstanceProcAddr, Str8_Lit("Failed to load the vkGetInstanceProcAddr proc"));
    
    return true;
}

static void VK_Unload_Library()
{
    FreeLibrary(GLibrary);
}

static VkSurfaceKHR VK_Win32_Create_Surface(HWND Window)
{
    vk_context* Context = VK_Get_Context();
    VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
    SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    SurfaceCreateInfo.hinstance = GetModuleHandleA(NULL);
    SurfaceCreateInfo.hwnd = Window;
    
    VkSurfaceKHR Surface;
    VK_Warning_Check(vkCreateWin32SurfaceKHR(Context->Instance, &SurfaceCreateInfo, VK_NULL_HANDLE, &Surface), 
                     Str8_Lit("Failed to create the win32 surface"));
    return Surface;
}

static HWND GDummyWindow;
static VkSurfaceKHR GDummySurface;

static bool VK_Begin_Presentation_Engine_Detection_Support()
{
    WNDCLASS WindowClass = {};
    WindowClass.lpfnWndProc = DefWindowProc;
    WindowClass.lpszClassName = "Win32_Dummy_Class";
    Bool_Warning_Check(RegisterClassA(&WindowClass), Str8_Lit("Failed to register the win32 dummy window class"));
    
    GDummyWindow = CreateWindowExA(NULL, "Win32_Dummy_Class", "", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandleA(NULL), NULL);
    Bool_Warning_Check(GDummyWindow, Str8_Lit("Failed to create the win32 dummy window for presentation support"));
    
    GDummySurface = VK_Win32_Create_Surface(GDummyWindow);
    Bool_Warning_Check(GDummyWindow, Str8_Lit("Failed to create the win32 dummy window surface"));
    
    return true;
}

static bool VK_Presentation_Engine_Queue_Family_Support(VkPhysicalDevice Device, uint32_t QueueFamilyIndex)
{
    VkBool32 Supported;
    VK_Warning_Check(vkGetPhysicalDeviceSurfaceSupportKHR(Device, QueueFamilyIndex, GDummySurface, &Supported), 
                     Str8_Lit("Failed to get the physical device surface support"));
    return (bool)Supported;
}

static void VK_End_Presentation_Engine_Detection_Support()
{
    vk_context* Context = VK_Get_Context();
    vkDestroySurfaceKHR(Context->Instance, GDummySurface, VK_NULL_HANDLE);
    DestroyWindow(GDummyWindow);
    UnregisterClassA("Win32_Dummy_Class", GetModuleHandleA(NULL));
}