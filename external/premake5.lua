project "GLFW"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    warnings "off"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "GLFW/include/GLFW/glfw3.h",
        "GLFW/include/GLFW/glfw3native.h",
        "GLFW/src/glfw_config.h",
        "GLFW/src/context.c",
        "GLFW/src/init.c",
        "GLFW/src/input.c",
        "GLFW/src/monitor.c",
        "GLFW/src/osmesa_context.c",

        "GLFW/src/null_init.c",
        "GLFW/src/null_joystick.c",
        "GLFW/src/null_monitor.c",
        "GLFW/src/null_window.c",

        "GLFW/src/platform.c",
        "GLFW/src/vulkan.c",
        "GLFW/src/window.c"
    }

    filter"system:linux"
        pic "on"

        systemversion "latest"

        files
        {
            "GLFW/src/x11_init.c",
            "GLFW/src/x11_monitor.c",
            "GLFW/src/x11_window.c",
            "GLFW/src/xkb_unicode.c",
            "GLFW/src/posix_module.c",
            "GLFW/src/posix_time.c",
            "GLFW/src/posix_thread.c",
            "GLFW/src/posix_poll.c",
            "GLFW/src/glx_context.c",
            "GLFW/src/egl_context.c",
            "GLFW/src/linux_joystick.c"
        }

        defines
        {
            "_GLFW_X11"
        }

    filter "system:windows"
        systemversion "latest"

        files
        {
            "GLFW/src/win32_init.c",
            "GLFW/src/win32_joystick.c",
            "GLFW/src/win32_module.c",
            "GLFW/src/win32_monitor.c",
            "GLFW/src/win32_time.c",
            "GLFW/src/win32_thread.c",
            "GLFW/src/win32_window.c",
            "GLFW/src/wgl_context.c",
            "GLFW/src/egl_context.c"
        }

        defines
        {
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:DebugOpt"
        runtime "Release"
        optimize "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
    staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"imgui/imconfig.h",
		"imgui/imgui.h",
		"imgui/imgui.cpp",
		"imgui/imgui_draw.cpp",
		"imgui/imgui_internal.h",
		"imgui/imgui_tables.cpp",
		"imgui/imgui_widgets.cpp",
		"imgui/imstb_rectpack.h",
		"imgui/imstb_textedit.h",
		"imgui/imstb_truetype.h",
		"imgui/imgui_demo.cpp"
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

    filter "configurations:DebugOpt"
		runtime "Release"
		optimize "on"
        symbols "off"

    filter "configurations:Release"
		runtime "Release"
		optimize "on"
        symbols "off"
