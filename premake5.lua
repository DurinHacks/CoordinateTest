-- premake5.lua
workspace "LotroCoordinateTest"
    architecture "x64"
    configurations
    {
        "Debug",
        "Release"
    }

local OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "LotroCoordinateTest"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir ("bin/" .. OutputDir)
    objdir ("build/" .. OutputDir)
    debugdir ("bin/" .. OutputDir)

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }
    filter "system:Windows"
        staticruntime "On"
        systemversion "latest"

    files
    {
        "inc/**.h",
        "src/**.cpp"
    }
    includedirs
    {
        "inc/"
    }

    filter "configurations:Debug"
        defines { "DU_DEBUG_BUILD" }
        symbols "On"
    filter "configurations:Release"
        defines { "DU_RELEASE_BUILD" }
        optimize "On"
    filter {}
