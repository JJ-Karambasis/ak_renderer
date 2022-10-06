@echo off

set ThirdPartyIncludePath=-I..\..\dependencies
set RendererIncludePath=-I..\..\include

set ExceptionFlags=-D_HAS_EXCEPTIONS=0 -GR-
set Common=-std:c++17 -nologo -Od -DDEBUG_BUILD -Z7 -FC -W4 -D -wd4100 -wd4065 -wd4201 -wd4189 -wd4996 -wd4530 -wd4505 -wd4100 %ExceptionFlags%

IF NOT EXIST .\bin (
	mkdir .\bin
)

pushd .\bin
cl %Common% %RendererIncludePath% %ThirdPartyIncludePath% -DUNIT_TESTS ..\..\source\ak_renderer.cpp -link user32.lib Advapi32.lib -out:Tests.exe

cl %Common% %RendererIncludePath% %ThirdPartyIncludePath% -LD ..\..\source\ak_renderer.cpp -link user32.lib Advapi32.lib -out:ak_renderer.dll
cl %Common% %RendererIncludePath% ..\test_app.cpp -link ak_renderer.lib user32.lib -out:test_app.exe
popd