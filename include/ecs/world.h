#pragma once

#include <cpprelude/hash_array.h>

#include <ecs/helper_structures/view.h>
#include <ecs/api.h>
#include <ecs/utility.h>
#include <ecs/helper_structures/entity_array.h>

namespace ecs
{	
	struct component_pool
	{
		utility::base_type_utils* utils = nullptr;
		cpprelude::memory_context* _context = nullptr;

		cpprelude::dynamic_array<void*> components;
		cpprelude::dynamic_array<bool> managed;
		cpprelude::dynamic_array<cpprelude::usize> sparse;
		cpprelude::dynamic_array<cpprelude::usize> dense;

		component_pool(cpprelude::memory_context* context)
			:_context(context),
			components(context),
			sparse(context),
			dense(context)
		{}

		component_pool()
			:_context(nullptr)
		{}

		void
			insert_at(cpprelude::usize entity_index, void* data_ptr, bool allocated_by_pool = true)
		{
			if (components.capacity() <= entity_index)
			{
				components.expand_back(2 * (entity_index + 1), nullptr);
				managed.expand_back(2 * (entity_index + 1), false);
				sparse.expand_back(2 * (entity_index + 1), INVALID_PLACE);
			}
			components[entity_index] = data_ptr;
			managed[entity_index] = allocated_by_pool;
			sparse[entity_index] = dense.count();
			dense.insert_back(entity_index);
		}

		void
			remove(cpprelude::usize entity_index)
		{
			if (entity_index < components.count())
			{
				if (managed[entity_index])
					utils->free(components[entity_index], _context);

				components[entity_index] = nullptr;
				cpprelude::usize dense_index = sparse[entity_index];
				std::swap(dense[dense_index], dense[dense.count()]);
				sparse[dense[dense_index]] = dense_index;
				sparse[entity_index] = -1;
				dense.remove_back();
			}
		}

		bool
			has(cpprelude::usize entity_index) const
		{
			return entity_index < components.count()
				&& components[entity_index] != nullptr;
		}

	};
	struct World
	{
		using entity_components = cpprelude::dynamic_array<cpprelude::dynamic_array <std::pair<cpprelude::usize, cpprelude::usize>>> ;

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
			if (!entities.has(internal_entity) || !type_exists<T>() || !has<T>(e))
				return;

			const auto type = utility::get_type_identifier<T>();
			auto& pool = component_types[type];
			pool.remove(internal_entity.id());
		}

		template<typename T> 
		T& 
		get(Entity e)
		{	
			const auto type = utility::get_type_identifier<T>();
			const auto& pool = component_pools[type].components;
			return *((T*)pool[e.id()]);
		}

		template<typename T>
		T&
		get(ID internal_entity)
		{
			const auto type = utility::get_type_identifier<T>();
			const auto& pool = component_pools[type].components;
			return *((T*)pool[internal_entity.id()]);
		}

		// this is the most basic thing I could think of now 
		// to provide this capability 
		//template<typename required, typename current>
		//required&
		//get_related_component(const Component<current>& component)
		//{
		//	const auto type = utility::get_type_identifier<required>();
		//	const auto& pool = component_pools[type].components;
		//	return *((required*)pool[component.entity_id.id()].data);
		//}

		//template<typename T>
		//component_view<T>
		//get_world_components()
		//{
		//	return component_view<T>(get_pool<T>());
		//}
			
		API_ECS Entity
		create_entity();
		
		API_ECS bool
		entity_alive(Entity entity);

		/*API_ECS entity_components_view
		get_all_entity_properties(Entity e);

		API_ECS entity_components_view
		get_all_entity_properties(ID internal_entity);

		API_ECS generic_component_view
		get_all_world_components();*/
				
		/*API_ECS sparse_unordered_set<Entity>&
		get_all_world_entities();*/
						
		API_ECS void
		kill_entity(Entity e);

		API_ECS void
		kill_entity(ID internal_entity);

		/*API_ECS void
		clean_up();
		
		~World()
		{
			clean_up();
		}*/
	};
}
