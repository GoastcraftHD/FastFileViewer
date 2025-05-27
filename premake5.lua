require("external/premake/premake_extensions/ecc/ecc")

workspace("FastFileViewer")
architecture("x86_64")
startproject("FastFileViewer")

configurations({
	"Debug",
	"Release",
})

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
VULKAN_SDK = os.getenv("VULKAN_SDK")

include("external/GLFW")
include("external/imgui")

project("FastFileViewer")
kind("ConsoleApp")
language("C++")
cppdialect("C++23")

targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

pchheader("FastFileViewerPCH.h")
pchsource("src/FastFileViewerPCH.cpp")

files({
	"src/**.h",
	"src/**.cpp",
	"external/glm/glm/**.hpp",
	"external/glm/glm/**.inl",
})

includedirs({
	"src/",
	"external/GLFW/include",
	"%{VULKAN_SDK}/include",
	"external/glm",
	"external/spdlog/include",
	"external/imgui",
})

libdirs({
	"%{VULKAN_SDK}/lib",
})

links({
	"GLFW",
})

defines({
	"GLFW_INCLUDE_NONE",
	"GLFW_INCLUDE_VULKAN",
})

-- Linux

filter("system:linux")
defines("FFV_LINUX")
toolset("clang")
buildoptions({
	"-Wall",
})
links({
	"vulkan",
})

filter({ "system:linux", "configurations:Release" })
buildoptions({
	"-Werror",
})

-- Windows

filter("system:windows")
defines("FFV_WINDOWS")
links({
	"vulkan-1",
})
buildoptions({
	"/W4",
	"/MP",
})

filter({ "system:windows", "configurations:Release" })
kind("WindowedApp")
buildoptions({
	"/WX",
})

-- Configurations

filter("configurations:Debug")
defines("FFV_DEBUG")
runtime("Debug")
symbols("on")

filter("configurations:Release")
defines("FFV_RELEASE")
runtime("Release")
optimize("on")
