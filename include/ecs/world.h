#pragma once

#include <cpprelude/hash_array.h>

#include <ecs/helper_structures/view.h>
#include <ecs/api.h>
#include <ecs/utility.h>


namespace ecs
{	

	struct World
	{
		using component_types_table = cpprelude::hash_array<const char*, cpprelude::dynamic_array<cpprelude::usize>, utility::details::hash_char>;

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
			component.dynamically_allocated = true;
		
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
			cpprelude::usize component_index = component_set.count() - 1;
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
		void
		remove_property(cpprelude::u64 entity_id)
		{
			const char* type = utility::get_type_name<T>();
				
			auto& entity_components = ledger[entity_id];
			auto& typed_components = type_table[type];
			cpprelude::usize last_index = entity_components.count() - 1;
			
			for (cpprelude::usize i = 0; i <= last_index; ++i)
			{
				auto index = entity_components[i];
				auto& component = component_set[index];
					
				if (component.utils->type != type)
					continue;

				auto& typed_components = type_table[component.utils->type];
				auto itr = std::find(typed_components.begin(), typed_components.end(), index);
				std::swap(typed_components[typed_components.count() - 1], *itr);
				typed_components.remove_back();

				if(component.dynamically_allocated)
					component.utils->free(component.data, _context);
				
				component_set.remove_by_index(index);

				std::swap(entity_components[i], entity_components[last_index--]);
			}

			entity_components.remove_back(entity_components.count() - last_index - 1);
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
		components_view<component_iterator<T>>
		get_entity_properties(cpprelude::u64 entity_id)
		{
			const char* type = utility::get_type_name<T>();
			auto& components = ledger[entity_id];
			
			component_iterator<T> begin(component_set.begin(), components.begin(), type, components.count());
			component_iterator<T> end(component_set.end(), components.end(), type, 0);
			
			return components_view<component_iterator<T>>(begin, end);
		}

		template<typename T>
		components_view<component_iterator<T>>
		get_world_components()
		{
			const char* type = utility::get_type_name<T>();
			auto& components = type_table[type];

			component_iterator<T> begin(component_set.begin(), components.begin(), type, components.count());
			component_iterator<T> end(component_set.end(), components.end(), type, 0);

			return components_view<component_iterator<T>>(begin, end);
		}

		API_ECS components_view<generic_component_iterator>
		get_all_entity_properties(cpprelude::u64 entity_id);
		
		API_ECS sparse_unordered_set<Component>&
		get_all_world_components();
				
		API_ECS sparse_unordered_set<Entity>&
		get_all_world_entities();
						
		API_ECS void
		kill_entity(cpprelude::u64 entity_id);

		API_ECS void
		clean_up();
		
		~World()
		{
			clean_up();
		}
	};
}
