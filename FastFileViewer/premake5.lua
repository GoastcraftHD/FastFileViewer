project "FastFileViewer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	VULKAN_SDK = os.getenv("VULKAN_SDK")

	includedirs
	{
		"src",
		"vendor/GLFW/include",
		"%{VULKAN_SDK}/Include"
	}

	libdirs
	{
		"%{VULKAN_SDK}/Lib"
	}

	links
	{
		"vulkan-1",
		"GLFW"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	filter "system:windows"
		systemversion "latest"
		defines "FFV_WINDOWS"

	filter "configurations:Debug"
		defines "FFV_DEBUG"
		links "shaderc_combinedd"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "FFV_RELEASE"
		links "shaderc_combined"
		runtime "Release"
		optimize "on"
