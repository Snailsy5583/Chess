project "stb"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "stb_image.h"
    }
    
    filter "system:windows"
        systemversion "latest"
        

	filter "configurations:Debug"
        runtime "Debug"
		buildoptions "/MTd"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
		buildoptions "/MT"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
		buildoptions "/MT"
        optimize "on"
        symbols "off"