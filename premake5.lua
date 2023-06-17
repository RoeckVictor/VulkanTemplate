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
IncludeDirs["GLFW"] = "MyFirstEngine/Libs/Glfw/include"
IncludeDirs["ImGui"] = "MyFirstEngine/Libs/ImGui"
IncludeDirs["VulkanSDK"] = "MyFirstEngine/Libs/VulkanSDK/1.3.243.0"
IncludeDirs["Glm"] = "MyFirstEngine/Libs/Glm"
IncludeDirs["Other"] = "MyFirstEngine/Libs/Other"

group "Dependencies"
	include "MyFirstEngine/Libs/Glfw"
	include "MyFirstEngine/Libs/ImGui"
group ""

project "MyFirstEngine"
	location "MyFirstEngine"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

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
		"%{prj.name}/Libs/Spdlog/include",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.VulkanSDK}/Include",
		"%{IncludeDirs.Glm}",
		"%{IncludeDirs.Other}"
	}

	links
	{
		"GLFW",
		"ImGui",
		"vulkan-1.lib"
	}
	
	libdirs
	{
		"%{IncludeDirs.VulkanSDK}/Lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"MFE_PLATFORM_WINDOWS",
			"MFE_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../Bin/" .. outputdir .. "/Sandbox/\"")
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
	staticruntime "off"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Intermediate/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{
		"MyFirstEngine/Libs/Spdlog/include",
		"MyFirstEngine/Source",
		"MyFirstEngine/Source/MyFirstEngine/Renderer",
		"%{IncludeDirs.VulkanSDK}/Include"
	}

	links
	{
		"MyFirstEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
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