@echo off

call ..\build.bat .\bin

set RendererIncludePath=-I..\..\include

IF NOT EXIST .\bin (
	mkdir .\bin
)

set ExceptionFlags=-D_HAS_EXCEPTIONS=0 -GR-
set Common=-std:c++17 -nologo -Od -DDEBUG_BUILD -Z7 -FC -W4 -D -wd4100 -wd4065 -wd4201 -wd4189 -wd4996 -wd4530 -wd4505 -wd4100 %ExceptionFlags%

pushd .\bin
cl %Common% %RendererIncludePath% ..\test_app.cpp -link ak_renderer.lib user32.lib -out:test_app.exe
popd