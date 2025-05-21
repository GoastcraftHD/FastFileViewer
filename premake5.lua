require("external/premake/premake_extensions/ecc/ecc")

workspace("FastFileViewer")
architecture("x86_64")
startproject("FastFileViewer")

configurations({
	"Debug",
	"DebugOpt", -- Same as Release but all asserts are still activated
	"Release",
})

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
VULKAN_SDK = os.getenv("VULKAN_SDK")
print(VULKAN_SDK)

include("external/GLFW")

project("FastFileViewer")
kind("ConsoleApp")
language("C++")
cppdialect("C++23")

targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

pchheader("FastFileViewerPCH.h")
pchsource("FastFileViewerPCH.cpp")

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
})

libdirs({
	"%{VULKAN_SDK}/lib",
})

links({
	"GLFW",
})

defines({
	"GLFW_INCLUDE_NONE",
})

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

filter("configurations:Debug")
defines("FFV_DEBUG")
runtime("Debug")
symbols("on")

filter("configurations:DebugOpt")
defines("FFV_DEBUG_OPT")
runtime("Release")
optimize("on")

filter("configurations:Release")
defines("FFV_RELEASE")
runtime("Release")
optimize("on")
