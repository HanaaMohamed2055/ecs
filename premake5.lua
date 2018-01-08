-- [[ function returning the sdk version of windows 10 --]]
function win10_sdk_version()
	cmd_file = io.popen("reg query \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots\" | C:\\Windows\\System32\\find.exe \"10.0\"", 'r')
	output = cmd_file:read("*all")
	cmd_file:close()
	out_root, out_leaf, out_ext = string.match(output, "(.-)([^\\]-([^%.]+))$")
	sdk_version = out_leaf:gsub("%s+", "")
	return sdk_version
end

bin_path = path.getabsolute("bin")
build_path = path.getabsolute("build")
ecs_path = path.getabsolute(".")
cpprelude_path = path.getabsolute("deps/cpprelude/cpprelude")

workspace "ecs"
	configurations {"debug", "release"}
	platforms {"x86", "x64"}
	location "build"
	startproject "scratch"

	include "ecs.lua"
	include "scratch/scratch.lua"
	include (cpprelude_path .. "/cpprelude.lua")
