#pragma once
#include <cpprelude/defines.h>
#include <cpprelude/platform.h>

#include <ecs/api.h>

namespace utility
{
	struct base_type_utils;
}

namespace ecs
{
	using _id_type = cpprelude::u64;
	using _version_type = cpprelude::u32;

	constexpr _id_type INVALID_ID = -1;

	struct World;
	
	struct ID
	{
		_id_type number = INVALID_ID;
		static constexpr auto entity_mask = 0xFFFFFFFFFF;
		static constexpr auto version_mask = 0xFFFFFF;
		static constexpr auto entity_bits = 40;
		
		bool
		operator==(const ID& other) const
 		{
			return number == other.number;
		}

		bool
		operator!=(const ID& other) const
		{
			return !operator==(other);
		}
		
		_id_type
		id() const
		{
			return number & entity_mask;
		}

		_version_type
		version() const
		{
			return _version_type((number >> entity_bits) & version_mask);
		}

		void
		increment_version()
		{
			const cpprelude::u32 new_version = ((number >> entity_bits) + 1) << entity_bits;
			number = (number & entity_mask) | new_version;
		}

		bool
		valid() const
		{
			return number != INVALID_ID;
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
		bool dynamically_allocated;
	};
}
