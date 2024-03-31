include "./vendor/premake/premake_customization/solution_items.lua"

workspace "FastFileViewer"
	architecture "x86_64"
    startproject "FastFileViewer"

	configurations
	{
		"Debug",
		"Release",
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "FastFileViewer/vendor/GLFW"
group ""

include "FastFileViewer"
