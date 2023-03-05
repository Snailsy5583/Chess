workspace "Chess"
	architecture "x64"
	startproject "Chess"
    
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDirs = {}
IncludeDirs["GLFW"] = "vendor/GLFW/include"
IncludeDirs["GLAD"] = "vendor/GLAD/include"
IncludeDirs["ImGui"] = "vendor/ImGui"
IncludeDirs["stb"] = "vendor/stb"

include "vendor/GLFW"
include "vendor/GLAD"
include "vendor/ImGui"
include "vendor/stb/stb"

project "Chess"
	location "Chess"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Main.cpp",
		"%{prj.name}/stb.cpp",
		"%{prj.name}/src/Board/Shaders/*",
		"%{prj.name}/src/Board/Pieces/Shaders/*",
		"%{prj.name}/src/Board/Pieces/Textures/*"
	}

	includedirs
	{
		"%{prj.name}/src",
		"vendor/spdlog/include",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.GLAD}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.stb}"
	}

	links
	{
		"GLFW",
		"GLAD",
		"ImGui",
		-- "stb",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "c++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"

	filter "configurations:Dist"
		optimize "On"