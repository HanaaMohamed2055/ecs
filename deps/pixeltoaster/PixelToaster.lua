project "pixeltoaster"
	language "C++"
	kind "StaticLib"
	targetdir (bin_path .. "/%{cfg.platform}/%{cfg.buildcfg}/")
	location (build_path .. "/pixeltoaster/")

	files {"include/**.h", "src/**.cpp"}

	includedirs {
		"include/",
		"C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"
	}

	if os.istarget("windows") then

		if os.getversion().majorversion == 10.0 then
			systemversion(win10_sdk_version())
		end
	end

	filter "configurations:debug"
		defines {"DEBUG", "PIXELTOASTER_NO_STL", "_MT", "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG", "PIXELTOASTER_NO_STL", "_MT", "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE"}
		optimize "On"

	filter "platforms:x86"
		architecture "x32"

	filter "platforms:x64"
		architecture "x64"
