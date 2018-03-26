#pragma once

#include <cpprelude/queue_array.h>

#include <ecs/helper_structures/view.h>
#include <ecs/api.h>
#include <ecs/utility.h>
#include <ecs/helper_structures/entity_array.h>


namespace ecs
{	
	
	struct World
	{
		entity_array entities ;
		cpprelude::dynamic_array<component_pool> component_pools;
		cpprelude::memory_context* _context;
		
		
		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context),
			entities(context),
			component_pools(context)
		{} 
				
		template<typename T, typename ... TArgs>
		Entity
		create_entity(TArgs&& ... args)
		{
			ID entity = entities.insert();
			add_property<T, TArgs...>(entity, std::forward<TArgs>(args)...);
			return Entity(entity, this);
		}

		template<typename T>
		Entity
		create_entity(const T& value)
		{
			ID entity = entities.insert();
			add_property<T>(entity, value);
			return Entity(entity, this);
		}

		template<typename T>
		Entity
		create_entity(T* value)
		{
			ID entity = entities.insert();
			add_property<T>(entity, value);
			return Entity(entity, this);
		}

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
			auto entity_id = entities.insert();
			entity.entity_id = entity_id;
			entity.world = this;
		}
						
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
			if (entities.has(internal_entity))
			{
				auto& pool = get_pool<T>(context);

				// constructing component itself
				void* data = pool._context->alloc<T>();
				new (data) T(std::forward<TArgs>(args)...);

				// registering component with the world and the entity 
				pool.insert_at(internal_entity.id(), data);

				return *(static_cast<T*>(data));
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
			if (entities.has(internal_entity))
			{
				auto& pool = get_pool<T>(context);

				// constructing component itself
				void* data = pool._context->alloc<T>();
				new (data) T(value);
				
				// registering component with the world and the entity 
				pool.insert_at(internal_entity.id(), data);

				return *(static_cast<T*>(data));
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
			if (entities.has(internal_entity))
			{
				auto& pool = get_pool<T>();
				
				// registering component with the world and the entity 
				pool.insert_at(internal_entity.id(), data, false);
			}
		}
			
		template<typename T>
		bool
		type_exists()
		{
			auto type = utility::get_type_identifier<T>();
			return (type < component_pools.count() && component_pools[type]._context != nullptr);
		}

		inline bool
		type_exists(cpprelude::usize type)
		{
			return (type < component_pools.count() && component_pools[type]._context != nullptr);
		}
		
		template<typename T>
		bool
		has(Entity e)
		{
			if (!entity_alive(e) || !type_exists<T>())
				return false;
			
			const auto type = utility::get_type_identifier<T>();
			const auto& pool = component_pools[type];
			
			return pool.has(e.id());
		}

		template<typename T>
		bool
		has(ID internal_entity)
		{
			if (!entities.has(internal_entity) || !type_exists<T>())
				return false;

			const auto type = utility::get_type_identifier<T>();
			const auto& pool = component_pools[type];

			return pool.has(internal_entity.id());
		}

		// has_all for multiple properties
		// still a lot slower than has<T>
		template<typename... Ts>
		bool
		has_all(Entity e)
		{
			if (!entity_alive(e))
				return false;

			auto types = utility::get_types_identifiers<Ts...>();
			for (auto type : types)
			{
				if (!type_exists(type) || !component_pools[type].has(e.id()))
					return false; 
			}

			return true;
		}

		template<typename... Ts>
		bool
		has_all(ID internal_entity)
		{
			if (!entities.has(internal_entity))
				return false;
			
			auto types = utility::get_types_identifiers<Ts...>();
			for (auto type : types)
			{
				if (!type_exists(type) || !component_pools[type].has(internal_entity.id()))
					return false;
			}
			return true;
		}

		template<typename T>
		void
		remove_property(Entity e)
		{
			if (!entity_alive(e) || !type_exists<T>() || !has<T>(e))
				return;

			const auto type = utility::get_type_identifier<T>();
			auto& pool = component_types[type];
			pool.remove(e.id());
		}

		template<typename T>
		void
		remove_property(ID internal_entity)
		{
			if (!entities.has(internal_entity) || !type_exists<T>() || !has<T>(internal_entity))
				return;

			const auto type = utility::get_type_identifier<T>();
			auto& pool = component_pools[type];
			pool.remove(internal_entity.id());
		}

		template<typename T> 
		T& 
		get(Entity e)
		{	
			const auto type = utility::get_type_identifier<T>();
			const auto& pool = component_pools[type];
			return *((T*)pool[e.id()].data);
		}

		template<typename T>
		T&
		get(ID internal_entity)
		{
			const auto type = utility::get_type_identifier<T>();
			const auto& pool = component_pools[type];
			return *((T*)pool[internal_entity.id()].data);
		}

		template<typename T>
		T&
		get(cpprelude::usize entity_index)
		{
			const auto type = utility::get_type_identifier<T>();
			const auto& pool = component_pools[type];
			return *((T*)pool[entity_index].data);
		}

		template<typename required, typename current>
		required&
		get_related_component(const Component<current>& component)
		{
			const auto type = utility::get_type_identifier<required>();
			const auto& pool = component_pools[type];
			return *((required*)pool[component.entity_id].data);
		}

		template<typename T>
		view<component_iterator<T>>
		get_world_components()
		{
			auto& pool = get_pool<T>();
			return view<component_iterator<T>>(pool.components.begin(), pool.components.end());
		}
			
		API_ECS Entity
		create_entity();
		
		API_ECS bool
		entity_alive(Entity entity);

		API_ECS view<entity_components_iterator>
		get_all_entity_properties(Entity e);

		API_ECS view<entity_components_iterator>
		get_all_entity_properties(ID internal_entity);

		API_ECS view<entity_components_iterator>
		get_all_entity_properties(cpprelude::usize entity_index);

		API_ECS view<generic_component_iterator>
		get_all_world_components();
				
		API_ECS entity_array&
		get_all_world_entities();
						
		API_ECS void
		kill_entity(Entity e);

		API_ECS void
		kill_entity(ID internal_entity);

		//API_ECS void
		//kill_entity(cpprelude::usize entity_index);

		/*API_ECS void
		clean_up();
		
		~World()
		{
			clean_up();
		}*/
	};
}
