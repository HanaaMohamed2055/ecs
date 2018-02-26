#pragma once
#include <cpprelude/hash_array.h>

#include <ecs/elements.h>
#include <ecs/utility.h>
#include <ecs/helper_structures/bag.h>
#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/api.h>


namespace ecs
{	
	namespace details
	{
		struct hash_char
		{
			inline cpprelude::usize
				operator()(const char* ptr) const
			{
				return cpprelude::hash_bytes(ptr, 8);
			}
		};
	}

	struct World
	{
		using component_types_table = cpprelude::hash_array<const char*, cpprelude::dynamic_array<cpprelude::usize>, details::hash_char>;

		bag<Entity> entity_bag;
		sparse_unordered_set<Component> components;
				
		cpprelude::hash_array<cpprelude::u64, cpprelude::dynamic_array<cpprelude::usize>> ledger;
		cpprelude::memory_context* _context;

		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context),
			ledger(context),
			entity_bag(context),
			components(context)
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

			components.insert(component);
			ledger[entity_id][component.utils->type].insert_back(components.count() - 1);
			
			return *(static_cast<T*>(component.data));	
		}

		template<typename T>
		void
		add_property(cpprelude::u64 entity_id, T* data)
		{
			ecs::Component component;
			component.data = data;
			component.utils = utility::get_type_utils<T>();

			components.insert(component);
			ledger[entity_id][component.utils->type].insert_back(components.count() - 1);
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
				for (auto index : components)
					components[index].utils->free(c.data, _context);

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
			auto component = components[ledger[entity_id][type][0]];
			
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
