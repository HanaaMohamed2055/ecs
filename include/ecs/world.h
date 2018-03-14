#pragma once

#include <cpprelude/hash_array.h>

#include <ecs/helper_structures/view.h>
#include <ecs/api.h>
#include <ecs/utility.h>

#define PREALLOCATED 1024

namespace ecs
{	
	struct component_pool
	{
		utility::base_type_utils* utils = nullptr;
		cpprelude::memory_context* _context = nullptr;
		
		sparse_unordered_set<Internal_Component> components;

		component_pool(cpprelude::memory_context* context)
			:_context(context),
			components(context)
		{}
	};

	struct World
	{
		using entity_components = cpprelude::dynamic_array<cpprelude::dynamic_array <std::pair<cpprelude::usize, cpprelude::usize>>> ;

		sparse_unordered_set<Entity> entity_set;
		cpprelude::dynamic_array<component_pool> component_types;
		cpprelude::memory_context* _context;
	
		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context),
			entity_set(context),
			component_types(context)
		{}
				
		API_ECS Entity
		create_entity();

		template<typename T>
		void
		add_entity(T& entity)
		{
			auto entity_id = entity_set.insert_one_more();
			entity.entity_id = entity_id;
			entity.world = this;
		}

		API_ECS bool
		entity_alive(Entity entity);
		
		template<typename T, typename ... TArgs>
		T&
		add_property(Entity e, TArgs&& ... args)
		{
			return add_property(e, _context, std::forward<TArgs>(args)...);
		}

		template<typename T, typename ... TArgs>
		T&
		add_property(Entity e, cpprelude::memory_context* context, TArgs&& ... args)
		{
			if (entity_alive(e))
			{
				cpprelude::usize type = utility::get_type_identifier<T>();

				// prealllocation at the beginning
				if (type >= component_types.count())
				{
					component_types.insert_back(component_pool(_context));
					component_types[type].utils = utility::get_type_utils<T>();
				}

				auto& pool = component_types[type];

				// constructing component itself
				Internal_Component component;
				component.data = pool._context->alloc<T>();
				new (component.data) T(std::forward<TArgs>(args)...);
				component.entity_id = e.id();
				component.dynamically_allocated = true;
			
				// registering component with the world and the entity 
				cpprelude::usize component_index = pool.components.insert(component);
				
				return *(static_cast<T*>(component.data));
			}
		}
				
		template<typename T>
		T&
		add_property(Entity e, const T& value)
		{
			return add_property(e, _context, value);
		}
		
		template<typename T>
		T&
		add_property(Entity e, cpprelude::memory_context* context, const T& value)
		{
			if (entity_alive(e))
			{
				cpprelude::usize type = utility::get_type_identifier<T>();

				// prealllocation at the beginning
				if (type >= component_types.count())
				{
					component_types.insert_back(component_pool(context));
					component_types[type].utils = utility::get_type_utils<T>();
				}

				auto& pool = component_types[type];

				// constructing component itself
				Internal_Component component;
				component.data = pool._context->alloc<T>();
				new (component.data) T(value);
				component.entity_id = e.id();
				component.dynamically_allocated = true;

				// registering component with the world and the entity 
				cpprelude::usize component_index = pool.components.insert(component);

				return *(static_cast<T*>(component.data));
			}
		}
		
		template<typename T>
		void
		add_property(Entity e, T* data)
		{
			if (entity_alive(e))
			{
				cpprelude::usize type = utility::get_type_identifier<T>();

				// prealllocation at the beginning
				if (type >= component_types.count())
				{
					component_types.insert_back(component_pool(_context));
					component_types[type].utils = utility::get_type_utils<T>();
				}

				auto& pool = component_types[type];

				// constructing component itself/////
				Internal_Component component;
				component.data = data;
				component.entity_id = e.id();
				component.dynamically_allocated = true;

				// registering component with the world and the entity 
				cpprelude::usize component_index = pool.components.insert(component);

				return *(static_cast<T*>(component.data));
			}
		}
				
		template<typename T>
		bool
		has(Entity e)
		{
			if (!entity_alive(e))
				return false;

			auto type = utility::get_type_identifier<T>();
			component_pool& pool = component_types[type];
			auto entity_id = e.id();

			for (auto component : pool.components)
			{
				if (component.entity_id == entity_id)
					return true;
			}

			return false;
		}
				
		/*template<typename T>
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
		}*/

		//API_ECS view<generic_component_iterator>
		//get_all_entity_properties(Entity e);
		//
		//API_ECS sparse_unordered_set<Component>&
		//get_all_world_components();
		//		
		//API_ECS sparse_unordered_set<Entity>&
		//get_all_world_entities();
						
		API_ECS void
		kill_entity(Entity e);

		//API_ECS void
		//clean_up();
		
		~World()
		{
			//clean_up();
		}
	};
}
