
workspace "Frostium"
	architecture "x64"
	startproject "PBR"

	configurations
	{
		"Debug_Vulkan",
		"Release_Vulkan",

		"SmolEngine_R",
		"SmolEngine_D"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glm"] = "include/External/glm"
IncludeDir["ImGui"] = "include/External/imgui"
IncludeDir["imgizmo"] = "include/External/imgizmo/src"

IncludeDir["GLFW"] = "vendor/glfw/include"
IncludeDir["Glad"] = "vendor/glad/include"
IncludeDir["vulkan"] = "include/External/vulkan/include"
IncludeDir["stb"] = "include/External/stb_image"
IncludeDir["ktx"] = "vendor/ktx/include"
IncludeDir["gli"] = "vendor/gli"
IncludeDir["tinygltf"] = "vendor/tinygltf"
IncludeDir["cereal"] = "vendor/cereal/include"
IncludeDir["ozz"] = "vendor/ozz-animation/include"

group "Dependencies"
include "include/External/imgui"
include "vendor/glfw"
include "vendor/glad"
include "vendor/spir_v_cross"
include "vendor/ktx"
include "vendor/imgizmo"
group ""


project "Frostium"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("vendor/libs/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stdafx.h"
	pchsource "src/stdafx.cpp"
	linkoptions { "/ignore:4006" }

	files
	{
		"include/**.h",
		"src/**.cpp",
		"src/**.h",

		"include/External/glm/glm/**.hpp",
		"include/External/glm/glm/**.inl",

		"include/External/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/implot/**.cpp",
	}

	includedirs
	{
		"vendor/",
		"vendor/icon_font_cpp_headers",
		
		"include/",
		"include/External",
		"include/External/implot/",
		"include/External/spdlog/include",

		"src/",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
        "%{IncludeDir.entt}",
		"%{IncludeDir.vulkan}",
		"%{IncludeDir.imgizmo}",
		"%{IncludeDir.ktx}",
		"%{IncludeDir.meta}",
		"%{IncludeDir.gli}",
		"%{IncludeDir.tinygltf}",
		"%{IncludeDir.taskflow}",
		"%{IncludeDir.cereal}",
		"%{IncludeDir.ozz}",
	}

	links 
	{ 
		"vendor/libs/" ..outputdir .. "/Glad/Glad.lib",
		"vendor/libs/" ..outputdir .. "/GLFW/GLFW.lib",
		"vendor/libs/" ..outputdir .. "/ImGizmo/ImGizmo.lib",
		"vendor/libs/" ..outputdir .. "/ImGui/ImGui.lib",
		"vendor/libs/" ..outputdir .. "/KTX-Tools/KTX-Tools.lib",
		"vendor/libs/" ..outputdir .. "/SPIRV-Cross/SPIRV-Cross.lib",

		"vendor/vulkan/libs/vulkan-1.lib",
	}

	defines
	{
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PLATFORM_WIN",
			"BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug_Vulkan"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		buildoptions "/Zm500"
		symbols "on"

		links 
		{ 
			"vendor/vulkan/libs/VkLayer_utils.lib",
			"vendor/vulkan/libs/shaderc_d.lib",
			"vendor/vulkan/libs/shaderc_util_d.lib",
			"vendor/vulkan/libs/glslang_d.lib",
			"vendor/vulkan/libs/SPIRV_d.lib",
			"vendor/vulkan/libs/SPIRV-Tools_d.lib",
			"vendor/vulkan/libs/SPIRV-Tools-opt_d.lib",
			"vendor/vulkan/libs/machineIndependent_d.lib",
			"vendor/vulkan/libs/genericCodeGen_d.lib",
			"vendor/vulkan/libs/OGLCompiler_d.lib",
			"vendor/vulkan/libs/OSDependent_d.lib",

			"vendor/ozz-animation/libs/ozz_animation_d.lib",
			"vendor/ozz-animation/libs/ozz_animation_offline_d.lib",
			"vendor/ozz-animation/libs/ozz_base_d.lib",
			"vendor/ozz-animation/libs/ozz_geometry_d.lib",
			"vendor/ozz-animation/libs/ozz_options_d.lib",
		}

		defines
		{
			"FROSTIUM_DEBUG"
		}

	filter "configurations:SmolEngine_D"
	buildoptions "/MDd"
	buildoptions "/bigobj"
	buildoptions "/Zm500"
	symbols "on"

	    links 
		{ 
			"vendor/vulkan/libs/VkLayer_utils.lib",
			"vendor/vulkan/libs/shaderc_d.lib",
			"vendor/vulkan/libs/shaderc_util_d.lib",
			"vendor/vulkan/libs/glslang_d.lib",
			"vendor/vulkan/libs/SPIRV_d.lib",
			"vendor/vulkan/libs/SPIRV-Tools_d.lib",
			"vendor/vulkan/libs/SPIRV-Tools-opt_d.lib",
			"vendor/vulkan/libs/machineIndependent_d.lib",
			"vendor/vulkan/libs/genericCodeGen_d.lib",
			"vendor/vulkan/libs/OGLCompiler_d.lib",
			"vendor/vulkan/libs/OSDependent_d.lib",

			"vendor/ozz-animation/libs/ozz_animation_d.lib",
			"vendor/ozz-animation/libs/ozz_animation_offline_d.lib",
			"vendor/ozz-animation/libs/ozz_base_d.lib",
			"vendor/ozz-animation/libs/ozz_geometry_d.lib",
			"vendor/ozz-animation/libs/ozz_options_d.lib",
		}
	
		defines
		{
			"FROSTIUM_SMOLENGINE_IMPL",
			"FROSTIUM_DEBUG"
		}

	filter "configurations:Release_Vulkan"
	buildoptions "/MD"
	buildoptions "/bigobj"
	buildoptions "/Zm500"
	optimize "on"

		links 
		{ 
			"vendor/vulkan/libs/shaderc.lib",
			"vendor/vulkan/libs/shaderc_util.lib",
			"vendor/vulkan/libs/glslang.lib",
			"vendor/vulkan/libs/SPIRV.lib",
			"vendor/vulkan/libs/SPIRV-Tools.lib",
			"vendor/vulkan/libs/SPIRV-Tools-opt.lib",
			"vendor/vulkan/libs/machineIndependent.lib",
			"vendor/vulkan/libs/genericCodeGen.lib",
			"vendor/vulkan/libs/OGLCompiler.lib",
			"vendor/vulkan/libs/OSDependent.lib",

			"vendor/ozz-animation/libs/ozz_animation_r.lib",
			"vendor/ozz-animation/libs/ozz_animation_offline_r.lib",
			"vendor/ozz-animation/libs/ozz_base_r.lib",
			"vendor/ozz-animation/libs/ozz_geometry_r.lib",
			"vendor/ozz-animation/libs/ozz_options_r.lib",
		}


	filter "configurations:SmolEngine_R"
	buildoptions "/MD"
	buildoptions "/bigobj"
	buildoptions "/Zm500"
	optimize "on"

		links 
		{ 
			"vendor/vulkan/libs/shaderc.lib",
			"vendor/vulkan/libs/shaderc_util.lib",
			"vendor/vulkan/libs/glslang.lib",
			"vendor/vulkan/libs/SPIRV.lib",
			"vendor/vulkan/libs/SPIRV-Tools.lib",
			"vendor/vulkan/libs/SPIRV-Tools-opt.lib",
			"vendor/vulkan/libs/machineIndependent.lib",
			"vendor/vulkan/libs/genericCodeGen.lib",
			"vendor/vulkan/libs/OGLCompiler.lib",
			"vendor/vulkan/libs/OSDependent.lib",

			"vendor/ozz-animation/libs/ozz_animation_r.lib",
			"vendor/ozz-animation/libs/ozz_animation_offline_r.lib",
			"vendor/ozz-animation/libs/ozz_base_r.lib",
			"vendor/ozz-animation/libs/ozz_geometry_r.lib",
			"vendor/ozz-animation/libs/ozz_options_r.lib",
		}

		defines
		{
		  "FROSTIUM_SMOLENGINE_IMPL"
		}

	filter "configurations:Release_OpenGL"
	buildoptions "/MD"
	buildoptions "/bigobj"
	buildoptions "/Zm500"
	optimize "on"

	   links 
	   { 
		"opengl32.lib"
	   }

	    defines
	   {
		 "FROSTIUM_OPENGL_IMPL"
	   }

--------------------------------------------------------------------------------- PBR

group "Samples"
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "PBR"
	kind "ConsoleApp"
	location "samples"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("vendor/libs/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"samples/PBR.h",
		"samples/PBR.cpp",
	}

	includedirs
	{
		"include/",
		
		"include/External",
		"include/External/vulkan/include",
		"include/External/spdlog/include",
		"include/External/glm/"
	}

	links
	{
		"bin/" ..outputdir .. "/Frostium/Frostium.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"PLATFORM_WIN"
		}

		filter "configurations:Debug_Vulkan"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		symbols "on"
	
		filter "configurations:Release_Vulkan"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"
	
		filter "configurations:SmolEngine_R"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"

		defines
		{
			"FROSTIUM_SMOLENGINE_IMPL"
		}
	
		filter "configurations:SmolEngine_D"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		symbols "on"

		defines
		{
			"FROSTIUM_SMOLENGINE_IMPL",
			"FROSTIUM_DEBUG"
		}


------------------------------------------------- 2D

project "2D"
	kind "ConsoleApp"
	location "samples"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("vendor/libs/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"samples/2D.h",
		"samples/2D.cpp",
	}

	includedirs
	{
		"include/",
		
		"include/External",
		"include/External/vulkan/include",
		"include/External/spdlog/include",
		"include/External/glm/"
	}

	links
	{
		"bin/" ..outputdir .. "/Frostium/Frostium.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"PLATFORM_WIN"
		}

		filter "configurations:Debug_Vulkan"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		symbols "on"
	
		filter "configurations:Release_Vulkan"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"
	
		filter "configurations:SmolEngine_R"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"

		defines
		{
			"FROSTIUM_SMOLENGINE_IMPL"
		}
	
		filter "configurations:SmolEngine_D"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		symbols "on"

		defines
		{
			"FROSTIUM_SMOLENGINE_IMPL",
			"FROSTIUM_DEBUG"
		}

	------------------------------------------------- 3D Animations

	project "Skinning"
	kind "ConsoleApp"
	location "samples"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("vendor/libs/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"samples/Skinning.h",
		"samples/Skinning.cpp",
	}

	includedirs
	{
		"include/",

		"include/External",
		"include/External/vulkan/include",
		"include/External/spdlog/include",
		"include/External/glm/"
	}

	links
	{
		"bin/" ..outputdir .. "/Frostium/Frostium.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"PLATFORM_WIN"
		}

		filter "configurations:Debug_Vulkan"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		symbols "on"
	
		filter "configurations:Release_Vulkan"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"
	
		filter "configurations:SmolEngine_R"
		buildoptions "/MD"
		buildoptions "/bigobj"
		optimize "on"

		defines
		{
			"FROSTIUM_SMOLENGINE_IMPL"
		}
	
		filter "configurations:SmolEngine_D"
		buildoptions "/MDd"
		buildoptions "/bigobj"
		symbols "on"

		defines
		{
			"FROSTIUM_SMOLENGINE_IMPL",
			"FROSTIUM_DEBUG"
		}

	group ""