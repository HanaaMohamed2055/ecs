#pragma once

#include "cpprelude/defines.h"

#ifdef OS_WINDOWS
#ifdef ECS_DLL
	#define API_ECS __declspec(dllexport)
#else
	#define API_ECS __declspec(dllimport)
#endif
#endif

#ifdef OS_LINUX
	#define API_ECS
#endif