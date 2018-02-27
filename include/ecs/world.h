#pragma once
#include <cpprelude/hash_array.h>


#include <ecs/helper_structures/bag.h>
#include <ecs/helper_structures/view.h>
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

		sparse_unordered_set<Entity> entity_set;
		sparse_unordered_set<Component> component_set;
				
		component_types_table type_table;
		cpprelude::hash_array<cpprelude::u64, cpprelude::dynamic_array<cpprelude::usize>> ledger;
		cpprelude::memory_context* _context;

		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context),
			ledger(context),
			entity_set(context),
			component_set(context),
			type_table(context)
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

			component_set.insert(component);
			cpprelude::usize component_index = component_set.count() - 1;
			ledger[entity_id].insert_back(component_index);
			type_table[component.utils->type].insert_back(component_index);

			return *(static_cast<T*>(component.data));	
		}

		template<typename T>
		void
		add_property(cpprelude::u64 entity_id, T* data)
		{
			ecs::Component component;
			component.data = data;
			component.utils = utility::get_type_utils<T>();

			component_set.insert(component);
			cpprelude::usize component_index = components.count() - 1;
			ledger[entity_id].insert_back(component_index);
			type_table[component.utils->type].insert_back(component_index);
		}

		
		template<typename T>
		bool
		has(cpprelude::u64 entity_id)
		{
			const char* type = utility::get_type_name<T>();
			auto& components = ledger[entity_id];
			
			for (auto index : components)
			{
				if (component_set[index].utils->type == type)
					return true;
			}

			return false;
		}
				
		template<typename T>
		bool
		remove_property(cpprelude::u64 entity_id)
		{
			if (has<T>(entity_id))
			{
				const char* type = utility::get_type_name<T>();
				
				auto& entity_components = ledger[entity_id];
				auto& typed_components = type_table[type];

				//TODO: find the intersection of the two arrays and remove it
		
				return true;
			}

			return false;
		}

		template<typename T>
		T& 
		get(cpprelude::u64 entity_id)
		{	
			const char* type = utility::get_type_name<T>();
			auto components = ledger[entity_id];
			
			for (auto index : components)
			{
				auto component = component_set[index];
				if (component.utils->type == type)
					return *(static_cast<T*>(component.data));
			}
		}
		
		template<typename T>
		components_view<T>
		get_all(cpprelude::u64 entity_id)
		{
			auto& components = ledger[entity_id];
			return components_view<T>(&components, &component_set);
		}

		generic_components_view
		get_all_properties(cpprelude::u64 entity_id)
		{
			auto& components = ledger[entity_id];
			return generic_components_view(&components, &component_set);
		}

		sparse_unordered_set<Component>&
		get_all_world_components()
		{
			return component_set;
		}
		
		sparse_unordered_set<Entity>&
		get_all_world_entities()
		{
			return entity_set;
		}
				
		/*API_ECS void
		kill_entity(cpprelude::u64 entity_id);

		API_ECS void
		clean_up();*/
		//
		//~World()
		//{
		//	clean_up();
		//}
	};
}
