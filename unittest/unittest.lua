local catch_path = "deps/Catch2"

project "unittest"
	language "C++"
	kind "ConsoleApp"
	targetdir (bin_path .. "/%{cfg.platform}/%{cfg.buildcfg}/")
	location  (build_path .. "/unittest/")

	files {"src/**.cpp"}

	includedirs {
		ecs_path .. "/include/",
		cpprelude_path .. "/include/",
		catch_path .. "/single_include/"
	}

	links {"cpprelude", "ecs"}

	if os.istarget("linux") then

		buildoptions {"-std=c++14", "-Wall"}
		linkoptions {"-pthread"}

	elseif os.istarget("windows") then

		if os.getversion().majorversion == 10.0 then
			systemversion(win10_sdk_version())
		end

		buildoptions {"/utf-8"}
	end

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "platforms:x86"
		architecture "x32"

	filter "platforms:x64"
		architecture "x64"