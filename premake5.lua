workspace "Blazr"
architecture "x64"
startproject "Sandbox"

configurations
{
  "Debug",
  "Release",
  "Dist"
}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Blazr"
location "Blazr"
kind "SharedLib"
language "C++"

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

files
{
  "%{prj.name}/src/**.h",
  "%{prj.name}/src/**.cpp"
}

includedirs
{
  "%{prj.name}/vendor/spdlog/include",
  "Blazr/src"
}

filter "system:windows"
cppdialect "C++17"
staticruntime "On"
systemversion "latest"

defines
{
  "BLZR_PLATFORM_WINDOWS",
  "BLZR_BUILD_DLL"
}

postbuildcommands{
  ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
}

filter "system:linux"
cppdialect "C++17"
staticruntime "On"
systemversion "latest"

defines
{
  "BLZR_PLATFORM_LINUX",
  "BLZR_BUILD_SO"
}

postbuildcommands{
  ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
}


filter "configurations:Debug"
defines "BLZR_DEBUG"
symbols "On"

filter "configurations:Release"
defines "BLZR_RELEASE"
optimize "On"

filter "configurations:Dist"
defines "BLZR_DIST"
optimize "On"

filter { "system:windows", "configurations:Release" }
buildoptions "/MT"

project "Sandbox"
location "Sandbox"
kind "ConsoleApp"
language "C++"

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

files
{
  "%{prj.name}/src/**.h",
  "%{prj.name}/src/**.cpp"
}

includedirs
{
  "Blazr/vendor/spdlog/include",
  "Blazr/src"
}

links {
  "Blazr"
}

filter "system:windows"
cppdialect "C++17"
staticruntime "On"
systemversion "latest"

defines
{
  "BLZR_PLATFORM_WINDOWS",
}

filter "system:linux"
cppdialect "C++17"
staticruntime "On"
systemversion "latest"

defines
{
  "BLZR_PLATFORM_LINUX",
}

filter "configurations:Debug"
defines "BLZR_DEBUG"
symbols "On"

filter "configurations:Release"
defines "BLZR_RELEASE"
optimize "On"

filter "configurations:Dist"
defines "BLZR_DIST"
optimize "On"

filter { "system:windows", "configurations:Release" }
buildoptions "/MT"
