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
		void
		add_entity(T& entity)
		{
			cpprelude::u64 entity_id = entity_set.insert(Entity());
			entity_set[entity_id].id = entity_id;
			entity_set[entity_id].world = this;
			
			entity.id = entity_id;
			entity.world = this;
		}

		template<typename T>
		T&
		add_property(Entity e, const T& value)
		{
			// simple check at the beginning that this entity instance belongs to this world
			if (e.id != INVALID_ID && e.world == this)
			{
				ecs::Component component;
				component.utils = utility::get_type_utils<T>();
				component.data = _context->alloc<T>();
				new (component.data) T(value);
				component.dynamically_allocated = true;
				component.entity = &(entity_set[e.id]);
								
				cpprelude::usize component_index = component_set.insert(component);
				ledger[e.id].insert_back(component_index);
				type_table[component.utils->type].insert_back(component_index);

				return *(static_cast<T*>(component.data));
			}
		}

		template<typename T>
		T&
		add_property(Entity e, T&& value)
		{
			if (e.id != INVALID_ID && e.world == this)
			{
				ecs::Component component;
				component.utils = utility::get_type_utils<T>();
				component.data = _context->alloc<T>();
				new (component.data) T(std::move(value));
				component.dynamically_allocated = true;
				component.entity = &(entity_set.[e.id]);

				cpprelude::usize component_index = component_set.insert(component);
				ledger[e.id].insert_back(component_index);
				type_table[component.utils->type].insert_back(component_index);

				return *(static_cast<T*>(component.data));
			}
		}

		template<typename T, typename ... TArgs>
		T&
		add_property(Entity e, TArgs&& ... args)
		{
			if (e.id != INVALID_ID && e.world == this)
			{
				ecs::Component component;
				component.utils = utility::get_type_utils<T>();
				component.data = _context->alloc<T>();
				new (component.data) T(std::forward<TArgs>(args)...);
				component.dynamically_allocated = true;
				component.entity = &(entity_set[e.id]);

				cpprelude::usize component_index = component_set.insert(component);
				ledger[e.id].insert_back(component_index);
				type_table[component.utils->type].insert_back(component_index);

				return *(static_cast<T*>(component.data));
			}
		}

		template<typename T>
		void
		add_property(Entity e, T* data)
		{
			if (e.id != INVALID_ID && e.world == this)
			{
				ecs::Component component;
				component.data = data;
				component.utils = utility::get_type_utils<T>();
				component.entity_id = e.id;
				component.entity = &(entity_set[e.id]);

				cpprelude::usize component_index = component_set.insert(component);
				ledger[e.id].insert_back(component_index);
				type_table[component.utils->type].insert_back(component_index);
			}
		}
				
		template<typename T>
		bool
		has(Entity e)
		{
			if (e.id != INVALID_ID && e.world == this)
			{
				const char* type = utility::get_type_name<T>();
				auto& components = ledger[e.id];

				for (auto index: components)
				{
					if (component_set[index].utils->type == type)
						return true;
				}
			}

			return false;
		}
				
		template<typename T>
		void
		remove_property(Entity e)
		{
			if (e.id == INVALID_ID || e.world != this)
				return;

			const char* type = utility::get_type_name<T>();
				
			auto& entity_components = ledger[e.id];
			auto& typed_components = type_table[type];
			cpprelude::isize last_index = entity_components.count() > 0 ? entity_components.count() - 1: -2;
			
			for (cpprelude::isize i = 0; i <= last_index; ++i)
			{
				auto index = entity_components[i];
				auto& component = component_set[index];
					
				if (component.utils->type != type)
					continue;

				auto itr = std::find(typed_components.begin(), typed_components.end(), index);
				std::swap(typed_components[typed_components.count() - 1], *itr);
				typed_components.remove_back();

				if(component.dynamically_allocated)
					component.utils->free(component.data, _context);
				
				component_set.remove(index);

				std::swap(entity_components[i], entity_components[last_index--]);
			}

			if(last_index > -2)
				entity_components.remove_back(entity_components.count() - last_index - 1);
		}

		template<typename T>
		T& 
		get(Entity e)
		{	
			if (e.id != INVALID_ID && e.world == this)
			{
				const char* type = utility::get_type_name<T>();
				auto components = ledger[e.id];

				for (auto index: components)
				{
					auto component = component_set[index];
					if (component.utils->type == type)
						return *(static_cast<T*>(component.data));
				}
			}
		}
		
		template<typename T>
		view<component_iterator<T>>
		get_entity_properties(Entity e)
		{
			if(e.id != INVALID_ID && e.world == this)
			{
				const char* type = utility::get_type_name<T>();
				auto& components = ledger[e.id];

				component_iterator<T> begin(&component_set, components.begin(), type, components.count());
				component_iterator<T> end(&component_set, components.end(), type, 0);

				return view<component_iterator<T>>(begin, end);
			}

			return view<component_iterator<T>>();
		}

		template<typename T>
		view<component_iterator<T>>
		get_world_components()
		{
			const char* type = utility::get_type_name<T>();
			auto& components = type_table[type];

			component_iterator<T> begin(&component_set, components.begin(), type, components.count());
			component_iterator<T> end(&component_set, components.end(), type, 0);

			return view<component_iterator<T>>(begin, end);
		}

		API_ECS view<generic_component_iterator>
		get_all_entity_properties(Entity e);
		
		API_ECS sparse_unordered_set<Component>&
		get_all_world_components();
				
		API_ECS sparse_unordered_set<Entity>&
		get_all_world_entities();
						
		API_ECS void
		kill_entity(Entity& e);

		API_ECS void
		clean_up();
		
		~World()
		{
			clean_up();
		}
	};
}
