#pragma once

#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>
#include <cpprelude/string.h>
#include <cpprelude/hash_array.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/elements.h>
#include <ecs/bag.h> 
#include <ecs/api.h>

namespace ecs
{	

	struct World
	{
		bag<Entity> entity_bag;
		bag<Internal_Component> component_bag;
		cpprelude::memory_context* _context;

		cpprelude::hash_array<cpprelude::u64, cpprelude::dynamic_array<cpprelude::u64>> entity_components;
		cpprelude::hash_array<cpprelude::string, cpprelude::dynamic_array<cpprelude::u64>> component_types;

		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context)
		{}

		API_ECS	Entity
		make_entity();
		

		template<typename T>
		Component<T>
		make_component(T data, cpprelude::u64 entity_id, cpprelude::string name = "")
		{
			cpprelude::u64 id = component_bag.insert(Internal_Component());

			// initialize component
			auto& component = component_bag[id];
			component.id = id;
			component.entity_id = entity_id;
			component._data = _context->alloc<T>();
			new (component._data) T(data);
			component._destroy = internal_component_dispose<T>;
			component.world = this;
			cpprelude::string key = cpprelude::string(typeid(T).name(), _context);
			component.type = key;
			component.name = name;

			// bind the component to the entity and cache its type
			if (entity_id != INVALID_ID)
				entity_components[entity_id].insert_back(id);
			component_types[key].insert_back(id);
			
			return &component;
		}


		template<typename T>
		cpprelude::dynamic_array<Component<T>> 
		get_entity_components(cpprelude::u64 entity_id)
		{
			cpprelude::dynamic_array<Component<T>> components; 
			
			if (entity_id != INVALID_ID)
			{
				cpprelude::string key(typeid(T).name(), _context);
				auto typed_components = component_types.lookup(key);

				if (typed_components != component_types.end())
				{
					auto& container = typed_components.value();
					for (auto index : container)
					{
						if (component_bag[index].entity_id != entity_id)
							continue;
						components.insert_back(&component_bag[index]);
					}
				}
			}
			return components;
		}


		API_ECS cpprelude::dynamic_array<Internal_Component*>
		get_all_entity_components(cpprelude::u64 entity_id);

		template<typename T>
		cpprelude::dynamic_array<Component<T>> 
		get_world_components()
		{
			cpprelude::dynamic_array<Component<T>> components;
			cpprelude::string key(typeid(T).name(), _context);
			auto typed_components = component_types.lookup(key);

			if (typed_components != component_types.end())
			{
				auto& container = typed_components.value();
				for (auto index : container)
				{
					components.insert_back(&component_bag[index]);
				}
			}
			return components;
		}

		API_ECS void
		kill_entity(cpprelude::u64 entity_id);

		
		API_ECS void
		remove_component(cpprelude::u64 component_id, bool unbind_from_entity = true);
		
		
		API_ECS void
		clean_up();
		

		~World()
		{
			clean_up();
		}
	};
}