@echo off
set "SDK_PATH=D:\Files\Autodidacte\Tutorials\Vulkan\VulkanTemplate\VulkanTutorial\Dependencies\VulkanSDK\1.3.243.0\Bin"
set "SHADER_DIR=%~dp0"

for /r "%SHADER_DIR%" %%f in (*.vert *.frag) do (
    echo Compiling: %%f
    "%SDK_PATH%\glslc.exe" "%%f" -o "%%~dpnxf.spv"
)
pause