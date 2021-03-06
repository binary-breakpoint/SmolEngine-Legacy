
workspace "SmolEngine"
	architecture "x64"
	startproject "SmolEngine-Editor"

	configurations
	{
		"Debug_Vulkan",
		"Release_Vulkan",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
include "vendor/glfw"
include "vendor/glad"
include "vendor/ktx"
include "vendor/imgizmo"
include "smolengine.external/imgui"
include "vendor/imgui-node-editor"
include "vendor/box2d"
include "vendor/ozz-animation"
include "vendor/soloud"
group ""

group "Engine"
include "smolengine"
include "smolengine.core"
include "smolengine.graphics"
include "smolengine.csharp"
group ""

group "Tools"
include "smolengine.editor"
group ""

group "Tests"
include "tests"
group ""