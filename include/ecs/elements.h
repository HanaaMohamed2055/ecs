#pragma once
#include <cpprelude/defines.h>
#include <cpprelude/platform.h>

namespace utility
{
	struct base_type_utils;
}

namespace ecs
{
	constexpr cpprelude::u64 INVALID_ID = -1UL;

	struct World;
	
	struct Entity
	{
		cpprelude::u64 id = INVALID_ID;
		World* world = nullptr;

		bool valid()
		{
			return id != INVALID_ID && world != nullptr;
		}
	};

	struct Component
	{
		void* data = nullptr;
		utility::base_type_utils* utils = nullptr;
		Entity* entity;
		bool dynamically_allocated = false;
	};
}
