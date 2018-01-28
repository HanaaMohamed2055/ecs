# pragma once

#include <functional>
#include "cpprelude/defines.h"
#include "cpprelude/dynamic_array.h"
#include "cpprelude/stack_array.h"

#define INVALID_ENTITY_ID 0

using namespace cpprelude;

namespace ecs 
{
	using entity_id = u32;
	using component_id = u16;

	struct entity
	{	
		entity_id eid;
		bool  is_valid;
		u8 component_count; // every entity should register to no more than 256 components

		entity()
		{
			eid = 0;
			is_valid = true;
			component_count = 0;
		}

		entity(entity_id eid)
		{
			eid = eid;
			is_valid = true;
			component_count = 0;
		}

		//this will act as a signal for the entity manager 
		//to destroy entity
		//deferred update???
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
		
		component_type* data;

		component(component_type* component_data)
		{
			data = component_data;
		}

	};

	/**The entity manager (so far) is responsible for creating, destroying entities,
	*reusing places of destroyed entities and applying a certain action
	*over all entities or entities registered to certain type(s) of components
	*/
	struct entity_component_manager
	{
		dynamic_array<entity> entities_index;
		stack_array<entity_id> free_places;

		entity* create()
		{
			entity_id new_id;

			if(free_places.empty())
				new_id = entities_index.count() + 1;
			else
			{	
				new_id = free_places.top();
				//this is a bool, decide later whether to check or not
				free_places.pop();
			}	

			entities_index.emplace_back(new_id);
			return &entities_index[entities_index.count() - 1];
		}	

		//bool valid(id entity_id){return true;}

		template<typename T>
		void register_component(entity_id eid, component<T> c)
		{

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
					free_places.push(entities_index[i].eid);	
					std::swap(entities_index[i], entities_index[entities_index.count() - 1]);
					entities_index.remove_back(1);
				}
			}
		}

	};

}