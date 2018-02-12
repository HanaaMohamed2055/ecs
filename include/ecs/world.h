#pragma once

#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>
#include <cpprelude/string.h>
#include <cpprelude/hash_array.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/elements.h>
#include <ecs/bag.h> 


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
		

		Entity make_entity()
		{
			cpprelude::u64 id = entity_bag.insert(Entity());
			entity_bag[id].id = id;
			entity_bag[id].world = this;
			return entity_bag[id];
		}

		template<typename T>
		Component<T> make_component(T data, cpprelude::u64 entity_id, cpprelude::string name = "")
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
			cpprelude::string key = typeid(T).name();
			component.type = key;
			component.name = name;

			// bind the component to the entity and cache its type
			if(entity_id != INVALID_ID)
				entity_components[entity_id].insert_back(id);
			component_types[key].insert_back(id);

			return &component;
		}


		//if no ID or invalid ID was supplied, all components of the type specified
		//will be returned
		template<typename T>
		cpprelude::dynamic_array<Component<T>> get_components_by_type(cpprelude::u64 entity_id = INVALID_ID)
		{
			cpprelude::dynamic_array<Component<T>> components; 
			cpprelude::string key = typeid(T).name();

			if(component_types.lookup(key) != component_types.end())
			{
				auto& container = component_types[key];
				for(auto index: container)
				{
					if(entity_id != INVALID_ID && component_bag[index].entity_id != entity_id)
						continue;
					components.insert_back(&component_bag[index]);
				}
			}

			return components;
		}


		cpprelude::dynamic_array<Internal_Component*> get_all_components(cpprelude::u64 entity_id)
		{
			cpprelude::dynamic_array<Internal_Component*> components;
			
			if(entity_id != INVALID_ID)
			{
				auto& container = entity_components[entity_id];
				for (auto index : container)
					components.insert_back(&component_bag[index]);
			}

			return components;
		}
	

		void kill_entity(cpprelude::u64 entity_id)
		{
			auto& component_ids = entity_components[entity_id];

			for (cpprelude::u64 cid: component_ids)
			{
				remove_component(cid, false);
			}
			component_ids.clear();
			entity_bag.remove(entity_id);
		}


		void remove_component(cpprelude::u64 component_id, bool unbind_from_entity = true)
		{
			auto& container = component_types[component_bag[component_id].type];
			
			component_bag[component_id].destroy(_context);
			component_bag.remove(component_id);

			auto itr = std::find(container.begin(), container.end(), component_id);
			if (itr != container.end())
			{
				auto index = itr - container.begin();
				std::swap(container[index], container[container.count() - 1]);
				container.remove_back();
			}
		}

		
		void clean_up()
		{
			// here, we should iterate on remaining valid components and destroy them 
			// free the memory allocated that will not be freed by ~bag()
			for (auto& component : component_bag)
				component.destroy(_context);
		}


		~World()
		{
			clean_up();
		}
	};
}