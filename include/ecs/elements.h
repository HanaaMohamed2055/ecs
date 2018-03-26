#pragma once
#include <cpprelude/defines.h>
#include <cpprelude/platform.h>

#include <ecs/api.h>

namespace ecs
{	
	constexpr cpprelude::usize INVALID_PLACE = -1;

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

	// internal component describes how the component is stored inside the world pool
	struct internal_component
	{
		void* data = nullptr;
		cpprelude::usize entity_index;
	};

	// this is the component that acts a convenient wrapper for data that knows to which entity it belongs
	template<typename T>
	struct Component
	{
		T* data;
		const cpprelude::usize entity_id;
		
		Component(T* data_ptr, cpprelude::usize entity_index)
			:data(data_ptr), entity_id(entity_index)
		{}

		Component(internal_component& c)
			:data((T*)c.data), entity_id(c.entity_index)
		{}

		T*
		operator->()
		{
			return data;
		}

		const T*
		operator->() const
		{
			return data;
		}

		T&
		operator*()
		{
			return *data;
		}

		const T&
		operator*() const
		{
			return *data;
		}

		bool
		operator==(const Component<T>& other)
		{
			return entity_id == other.entity_id
				&& *data == *(other.data);
		}

		bool
		operator!=(const Component<T>& other)
		{
			return !operator==(other)
		}
	};

	// generic component that knows its entity and type on iterating over all components in the system
	struct generic_component
	{
		void* data = nullptr;
		const cpprelude::usize entity_id;
		const char* type;
		
		generic_component(void* data_ptr, cpprelude::usize id, const char* data_type)
			:data(data_ptr), entity_id(id), type(data_type)
		{}
	};

	// entity_component that has to know only its type
	struct entity_component
	{
		void* data;
		const char* type;

		entity_component(void* data_ptr, const char* data_type)
			:data(data_ptr), type(data_type)
		{}
	};

}
