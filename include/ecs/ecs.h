# pragma once

#include <functional>

#include "cpprelude/defines.h"
#include "cpprelude/dynamic_array.h"
#include "cpprelude/queue_array.h"
#include "cpprelude/hash_array.h"
#include "cpprelude/string.h"

#define INVALID_ID 0

using namespace cpprelude;

namespace ecs 
{
	using entity_id = u32;
	using component_id = u32;

	//tick function

	struct entity
	{	
		entity_id eid;
		bool  is_valid;
		u32 component_count; // every entity should register to no more than 256 components

		entity()
		{
			eid = 0;
			is_valid = true;
			component_count = 0;
		}

		entity(entity_id id)
		{
			eid = id;
			is_valid = true;
			component_count = 0;
		}

		//this will act as a signal for the entity manager 
		//to destroy entity
		//deferred update??
		void remove(bool immediate = false) 
		{
			is_valid = false;
		}
	};	

	/** A component is a wrapper for whatever kind of data.
	* it can only be registered to by a single entity
	* An entity can register to any component ( Dynamic aggregation )
	*/
	template<typename T>
	struct component
	{
		using component_type = T;
		
		// component id must be unique within each entity		
		component_id cid;
		entity_id eid;

		component_type* data;

		component()
			:cid(INVALID_ID), eid(INVALID_ID), data(nullptr)
		{}

		component(component_type* component_data)
			:cid(INVALID_ID), eid(INVALID_ID), data(component_data)
		{}

		component(component_id id, component_type* component_data)
			:cid(id), eid(INVALID_ID), data(component_data)
		{}
	};

	/**The entity manager (so far) is responsible for creating, destroying entities,
	*reusing places of destroyed entities and applying a certain action
	*over all entities or entities registered to certain type(s) of components
	*/
	//this should take memory_context whatever which allocator has given  it -> resource manager
	struct entity_component_manager
	{
		//Allocator* _allocator;
		dynamic_array<entity> entities_index;
		queue_array<entity_id> free_places;
		hash_array<entity_id, dynamic_array<void*>> entities_components;
		
		/*template<typename T>
		hash_array <cpprelude::string, dynamic_array<component<T>>> types_map;*/

		entity* create()
		{
			entity_id new_id;

			if(free_places.empty())
				new_id = entities_index.count() + 1;
			else
			{	
				new_id = free_places.front();
				//this is a bool, decide later whether to check or not
				free_places.dequeue();
			}	

			entities_index.emplace_back(new_id);
			return &entities_index[entities_index.count() - 1];
		}	

		
		// Array of structures is enabled by default
		template<typename T>
		void bind(entity e, component<T>& c, bool AoS = true)
		{
			c.eid = e.eid;
			c.cid = e.component_count + 1;
			// here will lie some code that facilitates getting all components for a certain entity
			//ideally, we should use a hash array that  
			entities_components[e.eid].insert_back(&c);

		}
	
		template<typename return_type>
		void apply(std::function<return_type ()> func)
		{}

		template<typename component_type, typename return_type>
		void apply_on_type(std::function<return_type ()> func)
		{}

		void destroy()
		{
			for(size_t i = 0; i < entities_index.count(); ++i)
			{
				if(!entities_index[i].is_valid)
				{
					free_places.enqueue(entities_index[i].eid);	
					std::swap(entities_index[i], entities_index[entities_index.count() - 1]);
					entities_index.remove_back(1);
				}
			}
		}

	};

}