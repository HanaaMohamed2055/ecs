#pragma once
#include <cpprelude/hash_array.h>

#include <ecs/elements.h>
#include <ecs/utility.h>
#include <ecs/helper_structures/bag.h>
#include <ecs/api.h>


namespace ecs
{
	struct World
	{
		using component_types_table = cpprelude::hash_array<const char*, cpprelude::dynamic_array<Component>>;

		bag<Entity> entity_bag;
				
		cpprelude::hash_array<cpprelude::u64, component_types_table> ledger;
		cpprelude::memory_context* _context;

		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context),
			ledger(context),
			entity_bag(context)
		{}

		
		API_ECS Entity
		create_entity();

		template<typename T>
		T&
		add_property(cpprelude::u64 entity_id, T value)
		{
			ecs::Component component;
			component.utils = utility::get_type_utils<T>();
			component.data = _context->alloc<T>();
			new (component.data) T(value);

			ledger[entity_id][component.utils->type].insert_back(component);
			
			return *(static_cast<T*>(component.data));	
		}

		template<typename T>
		void
		add_property(cpprelude::u64 entity_id, T* data)
		{
			ecs::Component component;
			component.data = data;
			component.utils = utility::get_type_utils<T>();
		
			ledger[entity_id][component.utils->type].insert_back(component);
		}

		
		template<typename T>
		bool
		has(cpprelude::u64 entity_id)
		{
			const char* type = utility::get_type_name<T>();
			return ((ledger.lookup(entity_id) != ledger.end()) &&
				(ledger[entity_id].lookup(type) != ledger[entity_id].end()));
		}
				
		template<typename T>
		bool
		remove_property(cpprelude::u64 entity_id)
		{
			if (has<T>(entity_id))
			{
				const char* type = utility::get_type_name<T>();
				
				auto& components = ledger[entity_id][type];
				for (auto& c : components)
					c.utils->free(c.data, _context);

				ledger[entity_id].remove(type);
				return true;
			}

			return false;
		}

		template<typename T>
		T&
		get(cpprelude::u64 entity_id)
		{	
			const char* type = utility::get_type_name<T>();
			auto component = ledger[entity_id][type][0];
			
			return *(static_cast<T*>(component.data));
		}
		
		template<typename T>
		cpprelude::dynamic_array<Component>&
		get_all(cpprelude::u64 entity_id)
		{
			const char* type = utility::get_type_name<T>();
			auto& container = ledger[entity_id][type];
			
			return container;
		}
		
		API_ECS void
		kill_entity(cpprelude::u64 entity_id);

		API_ECS void
		clean_up();
		//
		//~World()
		//{
		//	clean_up();
		//}
	};
}
