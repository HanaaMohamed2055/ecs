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
		
		ID()
		{}

		ID(cpprelude::u64 value)
			:number(value)
		{}

		ID(cpprelude::u64 entity_id, cpprelude::u64 version)
		{
			number = (entity_id & entity_mask) | ((version & version_mask) << 40);
		}

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
			const cpprelude::u64 new_version = ((number >> entity_bits) + 1) << entity_bits;
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
		ID entity_id;
		World* world = nullptr;

		Entity()
		{}

		Entity(ID entity_id, World* entity_world)
			:entity_id(entity_id), world(entity_world)
		{}

		_id_type
		id() const
		{
			return entity_id.id();
		}

		_version_type
		version() const
		{
			return entity_id.version();
		}
	};

	struct Internal_Component
	{
		void* data = nullptr;
		ID entity_id;
		bool managed = false;
		
		Internal_Component(void* data_ptr, ID id, bool allocated_by_world)
			:data(data_ptr), entity_id(id), managed(allocated_by_world)
		{}
	};

	// still I am not so sure I will be using this or not
	template<typename T>
	struct Component
	{
		T* _data = nullptr;
		_id_type _entity_id;
		World* _world;	
		
		Component()
		{}

		Component(void* data, _id_type eid, World* world)
			:_data(data), _entity_id(eid), _world(world)
		{}

		Component(const Internal_Component& c, World* world)
			:_data(c.data), _entity_id(c.entity_id), _world(world)
		{}
	};
}
