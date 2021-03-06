project "ImGui"
	kind "StaticLib"
	language "C++"

	targetdir ("../../vendor/libs/" .. outputdir .. "/%{prj.name}")
	objdir ("../../vendor/libs/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
        "imgui_demo.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_widgets.cpp",
        "imgui_tables.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "on"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "on"

		filter "configurations:Debug_Vulkan"
		buildoptions "/MDd"
		symbols "on"
		
		filter "configurations:Release_Vulkan"
		buildoptions "/MD"
		optimize "full"
