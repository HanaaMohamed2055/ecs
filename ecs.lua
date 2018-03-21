project "ecs"
	kind "SharedLib"
	language "C++"
	targetdir (bin_path .. "/%{cfg.platform}/%{cfg.buildcfg}/")
	location (build_path .. "/ecs/")


	files {"include/**.h", "src/**.cpp"}

	includedirs {
		"include/",
		cpprelude_path .. "/include/",
		math_path .. "/include/"
	}

	links {"cpprelude"}
	
	if os.istarget("linux") then

		buildoptions {"-std=c++14", "-Wall", "-fno-rtti", "-fno-exceptions"}
		linkoptions {"-pthread"}

		filter "configurations:debug"
			linkoptions {"-rdynamic"}

	elseif os.istarget("windows") then

		if os.getversion().majorversion == 10.0 then
			systemversion(win10_sdk_version())
		end

		buildoptions {"/utf-8"}
	end


	filter "configurations:debug"
		targetsuffix "d"
		defines {"DEBUG", "ECS_DLL"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG", "ECS_DLL"}
		optimize "On"

	filter "platforms:x86"
		architecture "x32"

	filter "platforms:x64"
		architecture "x64"
