#pragma once
#include <cpprelude/defines.h>
#include <cpprelude/platform.h>

namespace utility
{
	struct base_type_utils;
}

namespace ecs
{
	// I doubt I need this now but let's leave it here until I am sure
	constexpr cpprelude::u64 INVALID_ID = -1UL;

	struct World;
	
	struct Entity
	{
		cpprelude::u64 id = INVALID_ID;
		World* world = nullptr;
	};

	struct Component
	{
		void* data = nullptr;
		utility::base_type_utils* utils = nullptr;
	};
}
