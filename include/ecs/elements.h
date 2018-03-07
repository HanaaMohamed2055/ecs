#pragma once
#include <cpprelude/defines.h>
#include <cpprelude/platform.h>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/api.h>

namespace utility
{
	struct base_type_utils;
}

namespace ecs
{
	using _id_type = cpprelude::u32;
	using _version_type = cpprelude::u8;

	constexpr _id_type INVALID_ID = -1;

	struct World;
	
	struct ID
	{
		_id_type number = INVALID_ID;
		_version_type version = -1;
		
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

		Entity()
		{}

		Entity(ID entity_id, World* entity_world)
			:id(entity_id), world(entity_world)
		{}
	};

	struct Component
	{
		void* data = nullptr;
		ID entity_id;
		World* world;		
	};

	struct component_type_entry
	{
		utility::base_type_utils* utils = nullptr;
		cpprelude::memory_context* _context = nullptr;
		
		// bool indicates whether it was dynamically allocated or not
		sparse_unordered_set<std::pair<Component, bool>> component_set;

		component_type_entry(cpprelude::memory_context* context)
			:_context(context),
			component_set(_context)
		{}
	};
}
