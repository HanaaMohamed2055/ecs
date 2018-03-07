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
	
	struct ID
	{
		cpprelude::u32 number = INVALID_ID;
		cpprelude::u32 version = -1;
		
		ID&
		operator=(const ID& other)
		{
			number = other.number;
			version = other.version;
			return *this;
		}

		bool
		operator==(const ID& other)
		{
			return number == other.number && version == other.version;
		}

		bool
		operator!=(const ID& other)
		{
			return !operator==(other);
		}
		
		bool
		valid()
		{
			return number != INVALID_ID && version != -1;
		}
	};

	struct Entity
	{
		ID id;
		World* world = nullptr;
	};

	struct Component
	{
		void* data = nullptr;
		utility::base_type_utils* utils = nullptr;
		Entity* entity;
		bool dynamically_allocated = false;
	};
}
