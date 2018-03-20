#pragma once

#include <cpprelude/hash_array.h>

#include <ecs/helper_structures/view.h>
#include <ecs/api.h>
#include <ecs/utility.h>
#include <ecs/helper_structures/sparse_unordered_set.h>

#define PREALLOCATED 1024

namespace ecs
{		
	struct World
	{
		using entity_components = cpprelude::dynamic_array<cpprelude::dynamic_array <std::pair<cpprelude::usize, cpprelude::usize>>> ;

		sparse_unordered_set<Entity> entity_set;
		cpprelude::dynamic_array<component_pool> component_pools;
		cpprelude::memory_context* _context;
	
		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context),
			entity_set(context),
			component_pools(context)
		{} 
				
		API_ECS Entity
		create_entity();

		template<typename T>
		component_pool&
		get_pool(cpprelude::memory_context* context = cpprelude::platform->global_memory)
		{
			cpprelude::usize type = utility::get_type_identifier<T>();
			
			// prealllocation at the beginning
			if (type >= component_pools.capacity())
				component_pools.expand_back(type + 1);

			if (component_pools[type]._context == nullptr)
			{
				component_pools[type] = (component_pool(context));
				component_pools[type].utils = utility::get_type_utils<T>();
			}
			
			return component_pools[type];
		}
		
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
			if (e.world == this)
			{
				return add_property<T, TArgs...>(e.entity_id, _context, std::forward<TArgs>(args)...);
			}
		}

		template<typename T, typename ... TArgs>
		T&
		add_property(Entity e, cpprelude::memory_context* context, TArgs&& ... args)
		{
			if (e.world == this)
			{
				return add_property<T, TArgs...>(e.entity_id, context, std::forward<TArgs>(args)...);
			}
		}

		template<typename T, typename ... TArgs>
		T&
		add_property(ID internal_entity, TArgs&& ... args)
		{
			return add_property<T, TArgs...>(internal_entity, _context, std::forward<TArgs>(args)...);
		}

		template<typename T, typename ... TArgs>
		T&
		add_property(ID internal_entity, cpprelude::memory_context* context, TArgs&& ... args)
		{
			if (entity_set.has(internal_entity))
			{
				auto& pool = get_pool<T>(context);

				// constructing component itself
				Internal_Component component;
				component.data = pool._context->alloc<T>();
				new (component.data) T(std::forward<TArgs>(args)...);
				component.entity_id = internal_entity;
				component.managed = true;

				// registering component with the world and the entity 
				pool.components.insert_at(internal_entity.id(), component);

				return *(static_cast<T*>(component.data));
			}
		}

		template<typename T>
		T&
		add_property(Entity e, const T& value)
		{
			if (e.world == this)
			{
				return add_property<T>(e.entity_id, _context, value);
			}
		}
		
		template<typename T>
		T&
		add_property(Entity e, cpprelude::memory_context* context, const T& value)
		{
			if (e.world == this)
			{
				return add_property<T>(e.entity_id, context, value);
			}
		}

		template<typename T>
		T&
		add_property(ID internal_entity, const T& value)
		{
			return add_property<T>(internal_entity, _context, value);
		}

		template<typename T>
		T&
		add_property(ID internal_entity, cpprelude::memory_context* context, const T& value)
		{
			if (entity_set.has(internal_entity))
			{
				auto& pool = get_pool<T>(context);

				// constructing component itself
				Internal_Component component;
				component.data = pool._context->alloc<T>();
				new (component.data) T(value);
				component.entity_id = internal_entity;
				component.managed = true;

				// registering component with the world and the entity 
				pool.components.insert_at(internal_entity.id(), component);

				return *(static_cast<T*>(component.data));
			}
		}

		template<typename T>
		void
		add_property(Entity e, T* data)
		{
			if (e.world == this)
			{
				return add_property<T>(e.entity_id, data);
			}
		}

		template<typename T>
		void
		add_property(ID internal_entity, T* data)
		{
			if (entity_set.has(internal_entity))
			{
				auto& pool = get_pool<T>();

				// constructing component itself/////
				Internal_Component component;
				component.data = data;
				component.entity_id = internal_entity;
				component.managed = true;

				// registering component with the world and the entity 
				pool.components.insert_at(internal_entity.id(), component);

				return *(static_cast<T*>(component.data));
			}
		}
			
		template<typename T>
		bool
		type_exists()
		{
			auto type = utility::get_type_identifier<T>();
			return (type < component_pools.count() && component_pools[type]._context != nullptr);
		}

		template<typename T>
		bool
		has(Entity e)
		{
			if (!entity_alive(e) || !type_exists<T>())
				return false;
			
			auto type = utility::get_type_identifier<T>();
			auto pool = component_pools[type].components;
			
			return pool.has(e.id());
		}

		template<typename T>
		bool
		has(ID internal_entity)
		{
			if (!entity_set.has(internal_entity) || !type_exists<T>())
				return false;

			auto type = utility::get_type_identifier<T>();
			auto pool = component_pools[type].components;

			return pool.has(internal_entity.id());
		}

		template<typename T>
		void
		remove_property(Entity e)
		{
			if (!entity_alive(e) || !type_exists<T>() || !has<T>(e))
				return;

			auto type = utility::get_type_identifier<T>();
			auto& pool = component_types[type].components;
			pool.remove(e.id());
		}

		template<typename T>
		void
		remove_property(ID internal_entity)
		{
			if (!entity_set.has(internal_entity) || !type_exists<T>() || !has<T>(e))
				return;

			auto type = utility::get_type_identifier<T>();
			auto& pool = component_types[type].components;
			pool.remove(internal_entity.id());
		}

		template<typename T> 
		T& 
		get(Entity e)
		{	
			auto type = utility::get_type_identifier<T>();
			auto pool = component_pools[type].components;
			return *((T*)pool[e.id()].data);
		}

		template<typename T>
		T&
		get(ID internal_entity)
		{
			auto type = utility::get_type_identifier<T>();
			auto pool = component_pools[type].components;
			return *((T*)pool[internal_entity.id()].data);
		}

		//TODO: this function needs to be modified 
		// to provide support for multi-component views
		template<typename T>
		component_view<T>
		get_world_components()
		{
			return component_view<T>(get_pool<T>());
		}
			
		API_ECS entity_components_view
		get_all_entity_properties(Entity e);

		API_ECS entity_components_view
		get_all_entity_properties(ID internal_entity);

		API_ECS generic_component_view
		get_all_world_components();
				
		API_ECS sparse_unordered_set<Entity>&
		get_all_world_entities();
						
		API_ECS void
		kill_entity(Entity e);

		API_ECS void
		kill_entity(ID internal_entity);

		API_ECS void
		clean_up();
		
		~World()
		{
			//clean_up();
		}
	};
}
