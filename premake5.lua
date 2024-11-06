workspace("Blazr")
architecture("x64")
startproject("Sandbox")

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
    windows = "Blazr/vendor/lua/windows/include"
}
IncludeDir["Sol2"] = "Blazr/vendor/sol2"
IncludeDir["json"] = "Blazr/vendor/json"

LibDir = {}
LibDir["GLFW"] = {
    linux = "Blazr/vendor/GLFW/bin/debug-linux-x86_64/GLFW",
    windows = "blazr/vendor/GLFW/bin/debug-windows-x86_64/GLFW",
}
LibDir["GLEW"] = {
    linux = "Blazr/vendor/glew/linux/lib",
    windows = "Blazr/vendor/glew/windows/lib/Release/x64"
}
LibDir["Lua"] = {
    linux = "Blazr/vendor/lua/linux",
    windows = "Blazr/vendor/lua/windows",
}

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
    "%{prj.name}/vendor/glm",
    "%{prj.name}/vendor/entt",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.json}",
    "%{IncludeDir.GLEW[os.host()]}",
    "%{IncludeDir.Lua[os.host()]}",
    "%{IncludeDir.Sol2}",
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
    buildoptions({"/utf-8"})

    libdirs({
        "%{LibDir.Lua.windows}",
    })

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
    })

filter("system:linux")
    cppdialect("C++20")
    staticruntime("On")
    systemversion("latest")

    libdirs({
        "%{LibDir.Lua.linux}",
    })

    links({
        "GL",
        "GLFW",
        "GLEW",
        "lua",
    })

    defines({
        "BLZR_PLATFORM_LINUX",
        "BLZR_BUILD_SO",
    })

    postbuildcommands({
        ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),
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

filter({ "system:windows", "configurations:Release" })
    buildoptions({"/MD", "/utf-8"})

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
    "Blazr/vendor/spdlog/include",
    "Blazr/src",
    "%{prj.name}/vendor/glm",
    "%{prj.name}/vendor/entt",
    "%{IncludeDir.Sol2}",
    "%{IncludeDir.json}",
    "%{IncludeDir.GLEW[os.host()]}",
})

links({
    "Blazr",
})

filter("system:windows")
    cppdialect("C++20")
    staticruntime("On")
    systemversion("latest")
    buildoptions({"/utf-8"})

    defines({
        "BLZR_PLATFORM_WINDOWS",
    })

filter("system:linux")
    cppdialect("C++20")
    staticruntime("On")
    systemversion("latest")

    defines({
        "BLZR_PLATFORM_LINUX",
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

filter({ "system:windows", "configurations:Release" })
    buildoptions({"/MD", "/utf-8"})
