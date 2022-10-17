@echo off

set OutputPath=%1
set CurrentPath=%~dp0
set CompileGlslang=1

If not defined OutputPath (set OutputPath=.\test_app\bin)

set ThirdPartyPath=%CurrentPath%\dependencies

set VulkanIncludePath=-I%VULKAN_SDK%\include
set ThirdPartyIncludePath=-I%ThirdPartyPath%
set RendererIncludePath=-I%CurrentPath%\include

set ExceptionFlags=-D_HAS_EXCEPTIONS=0 -GR-
set Common=-std:c++17 -nologo -Od -DDEBUG_BUILD -Z7 -FC -W4 -D -wd4100 -wd4065 -wd4201 -wd4189 -wd4996 -wd4530 -wd4505 -wd4100 %ExceptionFlags%

IF NOT EXIST %OutputPath% (
	mkdir %OutputPath%
)

IF %CompileGlslang% == 1 (
	IF NOT EXIST %OutputPath%\glslang mkdir %OutputPath%\glslang

	pushd %OutputPath%\glslang
	
	cl %Common% -wd4458 -wd4244 -wd4702 -wd4389 -wd4457 -wd4456 -wd4701 -wd4267 -wd4127 -DGLSLANG_VERSION_MAJOR=1 	-DGLSLANG_VERSION_MINOR=0 -DGLSLANG_VERSION_PATCH=0 -DGLSLANG_VERSION_FLAVOR=\"0\" -LD -c ^
	%ThirdPartyPath%\glslang\SPIRV\GlslangToSpv.cpp ^
	%ThirdPartyPath%\glslang\SPIRV\SpvBuilder.cpp ^
	%ThirdPartyPath%\glslang\SPIRV\CInterface\spirv_c_interface.cpp ^
	%ThirdPartyPath%\glslang\glslang\CInterface\glslang_c_interface.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\preprocessor\Pp.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\preprocessor\PpAtom.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\preprocessor\PpContext.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\preprocessor\PpScanner.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\preprocessor\PpTokens.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\attribute.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\Constant.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\glslang_tab.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\InfoSink.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\Initialize.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\Intermediate.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\IntermOut.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\IntermTraverse.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\iomapper.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\limits.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\linkValidate.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\parseConst.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\ParseContextBase.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\ParseHelper.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\PoolAlloc.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\propagateNoContraction.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\reflection.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\RemoveTree.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\Scan.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\SpirvIntrinsics.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\SymbolTable.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\Versions.cpp ^
	%ThirdPartyPath%\glslang\glslang\MachineIndependent\ShaderLang.cpp ^
	%ThirdPartyPath%\glslang\glslang\OSDependent\Windows\ossource.cpp ^
	%ThirdPartyPath%\glslang\glslang\GenericCodeGen\CodeGen.cpp ^
	%ThirdPartyPath%\glslang\glslang\GenericCodeGen\Link.cpp ^
	%ThirdPartyPath%\glslang\OGLCompilersDLL\InitializeDll.cpp

	lib -nologo -out:..\glslang.lib *.obj

	popd

	RMDIR /S /Q %OutputPath%\glslang
)

pushd %OutputPath%

cl %Common% %RendererIncludePath% %ThirdPartyIncludePath% %VulkanIncludePath% -DUNIT_TESTS ..\..\source\ak_renderer.cpp -link user32.lib Advapi32.lib glslang.lib -out:Tests.exe

cl %Common% %RendererIncludePath% %ThirdPartyIncludePath% %VulkanIncludePath% -LD ..\..\source\ak_renderer.cpp -link user32.lib Advapi32.lib glslang.lib -out:ak_renderer.dll
popd