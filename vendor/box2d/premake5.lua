project "Box2D"
	kind "StaticLib"
	language "C++"
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    targetdir ("../libs/" .. outputdir .. "/%{prj.name}")
    objdir ("../libs/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/collision/**.h",
		"src/collision/**.cpp",
		"src/common/**.h",
		"src/common/**.cpp",
		"src/dynamics/**.h",
		"src/dynamics/**.cpp",
		"src/rope/**.h",
		"src/rope/**.cpp"
	}

	includedirs
	{
		"../../smolengine.external/box_2D/include/"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "off"

		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug_Vulkan"
	symbols "on"


	filter "configurations:Release_Vulkan"
	optimize "full"


