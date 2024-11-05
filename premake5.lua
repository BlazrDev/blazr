workspace("Blazr")
architecture("x64")
startproject("Sandbox")
function getAbsolutePath()
    local projectRoot = os.getcwd()  -- Get the current directory
    local relativePath = "Blazr/vendor/lua/linux"
    return path.getabsolute(path.join(projectRoot, relativePath))
end
local absoluteLuaPath = getAbsolutePath()


configurations({
    "debug",
    "release",
    "dist",
})

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includedir = {}
includedir["glfw"] = "Blazr/vendor/GLFW/include"
includedir["glew"] = {
    linux = "Blazr/vendor/glew/linux/include",
    windows = "Blazr/vendor/glew/windows/include"
}
includedir["lua"] = {
    linux = "Blazr/vendor/lua/linux/include",
    windows = "Blazr/vendor/lua/windows/include"
}
includedir["sol2"] = "Blazr/vendor/sol2"
includedir["glm"] = "Blazr/vendor/glm"
includedir["entt"] = "Blazr/vendor/entt"

libdir = {}
libdir["glfw"] = {
    linux = "Blazr/vendor/GLFW/bin/debug-linux-x86_64/GLFW",
    windows = "Blazr/vendor/GLFW/bin/debug-windows-x86_64/GLFW",
}
libdir["glew"] = {
    linux = "Blazr/vendor/glew/linux/lib",
    windows = "Blazr/vendor/glew/windows/lib/release/x64"
}
libdir["lua"] = {
    linux = "Blazr/vendor/lua/linux",
    windows = "Blazr/vendor/lua/windows"
}
libdir["Blazr"] = {
    linux = "bin/debug-linux-x86_64/Blazr",
    windows = "bin/debug-windows-x86_64/Blazr",
}

-- function to build glew on linux
function build_glew()
    if os.host() == "linux" then
        os.execute("cd Blazr/vendor/glew/linux && make")
    end
end

-- call build_glew function for linux only
if os.host() == "linux" then
    build_glew()
end

include("Blazr/vendor/GLFW")

project("Blazr")
location("Blazr")
kind("sharedlib")
language("c++")

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

pchheader("blzrpch.h")
pchsource("Blazr/src/blzrpch.cpp")

files({
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
})

includedirs({
    "%{prj.name}/vendor/spdlog/include",
    "%{prj.name}/src",
    "%{includedir.glfw}",
    "%{includedir.glew[os.host()]}",
    "%{includedir.lua[os.host()]}", -- include lua directory based on OS
    "%{includedir.sol2}",
    "%{includedir.glm}",
    "%{includedir.entt}",
})

libdirs({
    "%{libdir.glfw[os.host()]}",
    "%{libdir.glew[os.host()]}",
    "%{libdir.lua[os.host()]}", -- add lua library path based on OS
})

filter("system:windows")
cppdialect("c++20")
staticruntime("on")
systemversion("latest")

links({
    "opengl32",
    "GLFW",
    "glew32s",
    "lua53"
})

defines({
    "BLZR_PLATFORM_WINDOWS",
    "BLZR_BUILD_DLL",
    "GLEW_STATIC",
})

postbuildcommands({
    ("{copy} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),
})

filter("system:linux")
cppdialect("c++20")
staticruntime("on")
systemversion("latest")

-- Explicitly use static library for lua53
links({
    "GL",
    "GLFW",
    "GLEW",
    "lua53"
})

defines({
    "BLZR_PLATFORM_LINUX",
    "BLZR_BUILD_SO",
    "GLEW_STATIC",
})

postbuildcommands({
    ("{copy} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),
})

project("Sandbox")
location("Sandbox")
kind("consoleapp")
language("c++")

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

files({
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
})

includedirs({
    "Blazr/vendor/spdlog/include",
    "Blazr/src",
    "%{includedir.sol2}",
    "%{includedir.glew[os.host()]}",
    "%{includedir.entt}",
    "%{includedir.glm}",
})

libdirs({
    "%{libdir.Blazr[os.host()]}",
    "%{libdir.lua[os.host()]}",
})

links({
    "Blazr",
    "lua53",
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

includedir["imgui"] = "ImGui/src"
includedir["imgui_backends"] = "ImGui/src/backends"

project("ImGui")
location("Blazr/ImGui")
kind("staticlib")
language("c++")

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

files({
    "%{includedir.imgui}/imgui.cpp",
    "%{includedir.imgui}/imgui_draw.cpp",
    "%{includedir.imgui}/imgui_tables.cpp",
    "%{includedir.imgui}/imgui_widgets.cpp",
    "%{includedir.imgui_backends}/imgui_impl_glfw.cpp",
    "%{includedir.imgui_backends}/imgui_impl_opengl3.cpp"
})

includedirs({
    "%{includedir.imgui}",
    "%{includedir.imgui_backends}",
    "%{includedir.glfw}",
    "%{includedir.glew[os.host()]}"
})

filter("system:windows")
    defines({ "IMGUI_IMPL_OPENGL_LOADER_GLEW" })
    links({ "opengl32", "glfw", "glew32s" })

filter("system:linux")
    links({ "GL", "glfw", "GLEW" })

filter("configurations:debug")
    runtime("debug")
    symbols("on")

filter("configurations:release")
    runtime("release")
    optimize("on")

filter("configurations:dist")
    runtime("release")
    optimize("on")

project("Editor")
location("Editor")
kind("consoleapp")
language("c++")

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

files({
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
})

includedirs({
    "Blazr/vendor/spdlog/include",
    "Blazr/src",
    "%{includedir.imgui}",
    "%{includedir.imgui_backends}",
    "%{includedir.glfw}",
    "%{includedir.glm}",
    "%{includedir.entt}",
    "%{includedir.sol2}",
    "%{includedir.glew[os.host()]}",
    "%{includedir.lua[os.host()]}",
})

-- Explicitly add the Lua library path
libdirs({
    "%{libdir.Blazr[os.host()]}",
    "%{libdir.glew[os.host()]}",
    "%{libdir.lua[os.host()]}",
})

-- Link with ImGui, Blazr, and lua53
links({
    "GL",
    "GLEW",
    "ImGui",
    "Blazr",
    "lua53",
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

linkoptions { "-Wl,-rpath=Blazr/vendor/lua/linux" }

filter("configurations:debug")
defines("blzr_debug")
symbols("on")

filter("configurations:release")
defines("blzr_release")
optimize("on")

filter("configurations:dist")
defines("blzr_dist")
optimize("on")
