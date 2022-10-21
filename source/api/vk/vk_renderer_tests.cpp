struct vk_tests
{
};

UTEST_F_SETUP(vk_tests)
{
    Bind_Core({});
    async_arena* MainArena = Create_Async_Arena(&GAllocator, Megabyte(1));
    Create_Thread_Context(MainArena);
    
    ak_renderer_init_result InitResults = VK_Init_Renderer();
    ASSERT_TRUE(InitResults.Initialized);
    ASSERT_TRUE(VK_Set_Device(InitResults.Devices.Devices[0]));
}

UTEST_F(vk_tests, VK_Convert_To_Glsl_Tests)
{
}

UTEST_F_TEARDOWN(vk_tests)
{
}