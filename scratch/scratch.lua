project "scratch"
	kind "ConsoleApp"
	language "C++"
	targetdir (bin_path .. "/%{cfg.platform}/%{cfg.buildcfg}/")
	location (build_path .. "/scratch/")

	files {"**.cpp"}

	includedirs {
		ecs_path .. "/include",
		cpprelude_path .. "/include",
	}

	links {"cpprelude", "ecs"}

	if os.istarget("linux") then

		buildoptions("-std=c++14", "-Wall", "-fno-rtti", "-fno-exceptions")
		linkoptions("-pthread")

	elseif os.istarget("windows") then

		if os.getversion().majorversion == 10.0 then
			systemversion(win10_sdk_version())
		end

		buildoptions {"/utf-8"}	
	end

	filter "configurations:debug"
		defines "DEBUG"
		symbols "On"

	filter "configurations:release"
		defines "NDEBUG"
		optimize "On"

	filter "platforms:x86"
		architecture "x32"

	filter "platforms:x64"
		architecture "x64"
