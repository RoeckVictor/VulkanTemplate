workspace "MyFirstEngine"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDirs	= {}
IncludeDirs["GLFW"] = "MyFirstEngine/Libs/Glfw/glfw/include"
IncludeDirs["ImGui"] = "MyFirstEngine/Libs/ImGui"
IncludeDirs["VulkanSDK"] = "MyFirstEngine/Libs/VulkanSDK/1.3.243.0"
IncludeDirs["Glm"] = "MyFirstEngine/Libs/Glm/glm/glm"
IncludeDirs["SpirvReflect"] = "MyFirstEngine/Libs/SPIRV-Reflect/SpirvReflect"
IncludeDirs["Spdlog"] = "MyFirstEngine/Libs/Spdlog/spdlog/include"
IncludeDirs["Other"] = "MyFirstEngine/Libs/Other"

group "Dependencies"
	include "MyFirstEngine/Libs/Glfw"
	include "MyFirstEngine/Libs/ImGui"
	include "MyFirstEngine/Libs/SPIRV-Reflect"
group ""

project "MyFirstEngine"
	location "MyFirstEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Intermediate/" .. outputdir .. "/%{prj.name}")

	pchheader "Mfepch.h"
	pchsource "MyFirstEngine/Source/Mfepch.cpp"

	files 
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{		
		"%{prj.name}/Source",
		"%{prj.name}/Source/MyFirstEngine/Renderer",
		"%{IncludeDirs.Spdlog}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.ImGui}/imgui",
		"%{IncludeDirs.VulkanSDK}/Include",
		"%{IncludeDirs.SpirvReflect}",
		"%{IncludeDirs.Glm}",
		"%{IncludeDirs.Other}"
	}

	links
	{
		"GLFW",
		"ImGui",
		"vulkan-1.lib",
		"SpirvReflect"
	}
	
	libdirs
	{
		"%{IncludeDirs.VulkanSDK}/Lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"MFE_PLATFORM_WINDOWS",
			"MFE_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "MFE_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "MFE_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "MFE_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Intermediate/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{
		"%{IncludeDirs.Spdlog}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.ImGui}/imgui",
		"MyFirstEngine/Source",
		"MyFirstEngine/Source/MyFirstEngine/Renderer",
		"%{IncludeDirs.VulkanSDK}/Include",
		"%{IncludeDirs.SpirvReflect}"
	}

	links
	{
		"MyFirstEngine"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"MFE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "MFE_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "MFE_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "MFE_DIST"
		runtime "Release"
		optimize "On"