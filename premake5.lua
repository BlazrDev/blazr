workspace("blazr")
architecture("x64")
startproject("sandbox")

configurations({
    "debug",
    "release",
    "dist",
})

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includedir = {}
includedir["glfw"] = "blazr/vendor/glfw/include"
includedir["glew"] = {
    linux = "blazr/vendor/glew/linux/include",
    windows = "blazr/vendor/glew/windows/include"
}
includedir["lua"] = {
    linux = "blazr/vendor/lua/linux/include",
    windows = "blazr/vendor/lua/windows/include"
}
includedir["sol2"] = "blazr/vendor/sol2"
includedir["glm"] = "blazr/vendor/glm"
includedir["entt"] = "blazr/vendor/entt"

libdir = {}
libdir["glfw"] = {
    linux = "blazr/vendor/glfw/bin/debug-linux-x86_64/glfw",
    windows = "blazr/vendor/glfw/bin/debug-windows-x86_64/glfw",
}
libdir["glew"] = {
    linux = "blazr/vendor/glew/linux/lib",
    windows = "blazr/vendor/glew/windows/lib/release/x64"
}
libdir["lua"] = {
    linux = "blazr/vendor/lua/linux",
    windows = "blazr/vendor/lua/windows"
}
libdir["blazr"] = {
    linux = "bin/debug-linux-x86_64/blazr",
    windows = "bin/debug-windows-x86_64/blazr",
}

-- function to build glew on linux
function build_glew()
    if os.host() == "linux" then
        os.execute("cd blazr/vendor/glew/linux && make")
    end
end

-- call build_glew function for linux only
if os.host() == "linux" then
    build_glew()
end

include("blazr/vendor/glfw")

project("blazr")
location("blazr")
kind("sharedlib")
language("c++")

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

pchheader("blzrpch.h")
pchsource("blazr/src/blzrpch.cpp")

files({
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
})

includedirs({
    "%{prj.name}/vendor/spdlog/include",
    "%{prj.name}/src",
    "%{includedir.glfw}",
    "%{includedir.glew[os.host()]}",
    "%{includedir.lua[os.host()]}", -- include lua directory based on os
    "%{includedir.sol2}",
    "%{includedir.glm}",
    "%{includedir.entt}",
})

libdirs({
    "%{libdir.glfw[os.host()]}",
    "%{libdir.glew[os.host()]}",
    "%{libdir.lua[os.host()]}", -- add lua library path based on os
})

filter("system:windows")
cppdialect("c++20")
staticruntime("on")
systemversion("latest")

links({
    "opengl32",
    "glfw",
    "glew32s",
    "lua53"
})

defines({
    "BLZR_PLATFORM_WINDOWS",
    "BLZR_BUILD_DLL",
    "GLEW_STATIC",
})

postbuildcommands({
    ("{copy} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/sandbox"),
})

filter("system:linux")
cppdialect("c++20")
staticruntime("on")
systemversion("latest")

links({
    "gl",
    "glfw3",
    "glew",
    "lua",
})

defines({
    "BLZR_PLATFORM_LINUX",
    "BLZR_BUILD_SO",
    "GLEW_STATIC",
})

postbuildcommands({
    ("{copy} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/sandbox"),
})

filter("configurations:debug")
defines("blzr_debug")
symbols("on")

filter("configurations:release")
defines("blzr_release")
optimize("on")

filter("configurations:dist")
defines("blzr_dist")
optimize("on")

filter({ "system:windows", "configurations:release" })
buildoptions("/md")

project("sandbox")
location("sandbox")
kind("consoleapp")
language("c++")

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

files({
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
})

includedirs({
    "blazr/vendor/spdlog/include",
    "blazr/src",
    "%{includedir.sol2}",
    "%{includedir.glew[os.host()]}",
})

libdirs({
    "%{libdir.blazr[os.host()]}", -- add lua library path based on os
})

links({
    "blazr",
})

filter("system:windows")
cppdialect("c++20")
staticruntime("on")
systemversion("latest")

defines({
    "BLZR_PLATFORM_WINDOWS",
})

filter("system:linux")
cppdialect("c++20")
staticruntime("on")
systemversion("latest")

defines({
    "BLZR_PLATFORM_LINUX",
})

filter("configurations:debug")
defines("blzr_debug")
symbols("on")

filter("configurations:release")
defines("blzr_release")
optimize("on")

filter("configurations:dist")
defines("blzr_dist")
optimize("on")

filter({ "system:windows", "configurations:release" })
buildoptions("/md")
