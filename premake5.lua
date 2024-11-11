workspace("Blazr")
architecture("x64")
startproject("Sandbox")

function getAbsolutePath()
    local projectRoot = os.getcwd()
    local relativePath = "Blazr/vendor/lua/linux"
    return path.getabsolute(path.join(projectRoot, relativePath))
end
local absoluteLuaPath = getAbsolutePath()

configurations({
    "Debug",
    "Release",
    "Dist",
})

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Blazr/vendor/GLFW/include"
IncludeDir["GLEW"] = {
    linux = "Blazr/vendor/glew/linux/include",
    windows = "Blazr/vendor/glew/windows/include",
}
IncludeDir["Lua"] = {
    linux = "Blazr/vendor/lua/linux/include",
    windows = "Blazr/vendor/lua/windows/include",
}
IncludeDir["Sol2"] = "Blazr/vendor/sol2"
IncludeDir["GLM"] = "Blazr/vendor/glm"
IncludeDir["EnTT"] = "Blazr/vendor/entt"
IncludeDir["Json"] = "Blazr/vendor/json"

LibDir = {}
LibDir["GLFW"] = {
    linux = "Blazr/vendor/GLFW/bin/Debug-linux-x86_64/GLFW",
    windows = "Blazr/vendor/GLFW/bin/Debug-windows-x86_64/GLFW",
}
LibDir["GLEW"] = {
    linux = "Blazr/vendor/glew/linux/lib",
    windows = "Blazr/vendor/glew/windows/lib/Release/x64",
}
LibDir["Lua"] = {
    linux = "Blazr/vendor/lua/linux",
    windows = "Blazr/vendor/lua/windows",
}
LibDir["Blazr"] = {
    linux = "bin/Debug-linux-x86_64/Blazr",
    windows = "bin/Debug-windows-x86_64/Blazr",
}

-- Function to build GLEW on Linux
function build_glew()
    if os.host() == "linux" then
        os.execute("cd Blazr/vendor/glew/linux && make")
    end
end

if os.host() == "linux" then
    build_glew()
end

include("Blazr/vendor/GLFW")

project("Blazr")
location("Blazr")
kind("SharedLib")
language("C++")

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
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.GLEW[os.host()]}",
    "%{IncludeDir.Lua[os.host()]}",
    "%{IncludeDir.Sol2}",
    "%{IncludeDir.GLM}",
    "%{IncludeDir.EnTT}",
    "%{IncludeDir.Json}",
})

libdirs({
    "%{LibDir.GLFW[os.host()]}",
    "%{LibDir.GLEW[os.host()]}",
    "%{LibDir.Lua[os.host()]}",
})

filter("system:windows")
cppdialect("C++20")
staticruntime("On")
systemversion("latest")

links({
    "OpenGL32",
    "GLFW",
    "glew32s",
    "lua53",
})

defines({
    "BLZR_PLATFORM_WINDOWS",
    "BLZR_BUILD_DLL",
    "GLEW_STATIC",
})

postbuildcommands({
    ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),
    ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Editor"),
})

filter("system:linux")
cppdialect("C++20")
staticruntime("On")
systemversion("latest")

links({
    "GL",
    "GLFW",
    "GLEW",
    "lua53",
})

linkoptions({ "-Wl,-rpath=Blazr/vendor/lua/linux" })

defines({
    "BLZR_PLATFORM_LINUX",
    "BLZR_BUILD_SO",
    "GLEW_STATIC",
})

postbuildcommands({
    ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),
    ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Editor"),
})

filter("configurations:Debug")
defines("BLZR_DEBUG")
symbols("On")

filter("configurations:Release")
defines("BLZR_RELEASE")
optimize("On")

filter("configurations:Dist")
defines("BLZR_DIST")
optimize("On")

project("Sandbox")
location("Sandbox")
kind("ConsoleApp")
language("C++")

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

files({
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
})

includedirs({
    "Blazr/vendor/glm",
    "Blazr/vendor/entt",
    "Blazr/vendor/spdlog/include",
    "Blazr/src",
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.ImGuiBackends}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.GLM}",
    "%{IncludeDir.EnTT}",
    "%{IncludeDir.Sol2}",
    "%{IncludeDir.Json}",
    "%{IncludeDir.GLEW[os.host()]}",
    "%{IncludeDir.Lua[os.host()]}",
})

libdirs({
    "%{LibDir.Blazr[os.host()]}",
    "%{LibDir.GLEW[os.host()]}",
    "%{LibDir.Lua[os.host()]}",
})

filter("system:windows")
cppdialect("C++20")
staticruntime("On")
systemversion("latest")

links({
    "OpenGL32",
    "glew32s",
    "ImGui",
    "Blazr",
    "lua53",
})

defines({
    "BLZR_PLATFORM_WINDOWS",
})

filter("system:linux")
cppdialect("C++20")
staticruntime("On")
systemversion("latest")

links({
    "GL",
    "GLEW",
    "ImGui",
    "Blazr",
    "lua53",
})

defines({
    "BLZR_PLATFORM_LINUX",
})

linkoptions({ "-Wl,-rpath=Blazr/vendor/lua/linux" })

filter("configurations:Debug")
defines("BLZR_DEBUG")
symbols("On")

filter("configurations:Release")
defines("BLZR_RELEASE")
optimize("On")

filter("configurations:Dist")
defines("BLZR_DIST")
optimize("On")

filter({ "system:windows", "configurations:Release" })
-- buildoptions("/MD")

IncludeDir["ImGui"] = "ImGui/src"
IncludeDir["ImGuiBackends"] = "ImGui/src/backends"

project("ImGui")
location("Blazr/ImGui")
kind("StaticLib")
language("C++")

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

files({
    "%{IncludeDir.ImGui}/imgui.cpp",
    "%{IncludeDir.ImGui}/imgui_draw.cpp",
    "%{IncludeDir.ImGui}/imgui_tables.cpp",
    "%{IncludeDir.ImGui}/imgui_widgets.cpp",
    "%{IncludeDir.ImGuiBackends}/imgui_impl_glfw.cpp",
    "%{IncludeDir.ImGuiBackends}/imgui_impl_opengl3.cpp",
})

includedirs({
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.GLEW[os.host()]}",
})

filter("system:windows")
defines({ "IMGUI_IMPL_OPENGL_LOADER_GLEW" })
links({ "opengl32", "glfw" })

filter("system:linux")
links({ "GL", "glfw", "GLEW" })

filter("configurations:Debug")
runtime("Debug")
symbols("On")

filter("configurations:Release")
runtime("Release")
optimize("On")

project("Editor")
location("Editor")
kind("ConsoleApp")
language("C++")

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("obj/" .. outputdir .. "/%{prj.name}")

files({
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
})

includedirs({
    "Blazr/vendor/spdlog/include",
    "Blazr/src",
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.ImGuiBackends}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.GLM}",
    "%{IncludeDir.EnTT}",
    "%{IncludeDir.Sol2}",
    "%{IncludeDir.Json}",
    "%{IncludeDir.GLEW[os.host()]}",
    "%{IncludeDir.Lua[os.host()]}",
})

libdirs({
    "%{LibDir.Blazr[os.host()]}",
    "%{LibDir.GLEW[os.host()]}",
    "%{LibDir.Lua[os.host()]}",
})

-- General Links
links({
    "ImGui",
    "Blazr",
})

-- Windows-specific configuration
filter("system:windows")
cppdialect("C++20")
systemversion("latest")

links({
    "OpenGL32",
    "glew32s",
    "lua53",
})

defines({
    "BLZR_PLATFORM_WINDOWS",
    "UNICODE",
    "_UNICODE",
})

-- Linux-specific configuration
filter("system:linux")
cppdialect("C++20")
staticruntime("On")
systemversion("latest")

links({
    "GL",
    "GLEW",
    "lua53",
})

defines({
    "BLZR_PLATFORM_LINUX",
})

linkoptions({ "-Wl,-rpath=Blazr/vendor/lua/linux" })

-- Debug Configuration
filter("configurations:Debug")
runtime("Debug")
defines("BLZR_DEBUG")
symbols("On")

-- Release Configuration
filter("configurations:Release")
runtime("Release")
defines("BLZR_RELEASE")
optimize("On")

-- Distribution Configuration
filter("configurations:Dist")
runtime("Release")
defines("BLZR_DIST")
optimize("On")

-- Linux-specific configuration
filter("system:linux")
cppdialect("C++20")
staticruntime("On")
systemversion("latest")

links({
    "GL",
    "GLEW",
    "lua53",
})

defines({
    "BLZR_PLATFORM_LINUX",
})

linkoptions({ "-Wl,-rpath=Blazr/vendor/lua/linux" })

-- Debug Configuration
filter("configurations:Debug")
defines("BLZR_DEBUG")
symbols("On")

-- Release Configuration
filter("configurations:Release")
defines("BLZR_RELEASE")
optimize("On")

-- Distribution Configuration
filter("configurations:Dist")
defines("BLZR_DIST")
optimize("On")