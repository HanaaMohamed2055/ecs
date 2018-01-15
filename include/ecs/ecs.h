# pragma once

#include <functional>
#include "cpprelude/defines.h"
#include "cpprelude/dynamic_array.h"
#include "cpprelude/stack_array.h"

using namespace cpprelude;

namespace ecs 
{
	using id = u32;

	struct entity
	{	
		id entity_id;
		bool  removed;
		u32 component_count;

		entity()
		{
			entity_id = 0;
			removed = false;
			component_count = 0;
		}

		entity(id eid)
		{
			entity_id = eid;
			removed = false;
			component_count = 0;
		}

		//this will act as a signal for the entity manager 
		//to destroy entity
		//deferred update???
		void remove(bool immediate = false) {};
	};	

	/** A component is a wrapper for whatever kind of data.
	* it can only be registered to by a single entity
	* An entity can register to any component ( Dynamic aggregation )
	*/
	template<typename T>
	struct component
	{
		using component_type = T;
		
		component_type* data;

		component(){};

		component(const component_type* data){};

	};

	/**The entity manager (so far) is responsible for creating, destroying entities,
	*reusing places of destroyed entities and applying a certain action
	*over all entities or entities registered to certain type(s) of components
	*/
	struct entity_component_manager
	{
		
		dynamic_array<entity> entities_index;
		stack_array<id> free_places;

		entity create(){

			id new_id;

			if(free_places.empty())
				new_id = entities_index.count() + 1;
			else
			{	
				new_id = free_places.top();
				//this is a bool, decide later whether to check or not
				free_places.pop();
			}	

			entity e(new_id);
			entities_index.insert_back(new_id);
			return e;
		}	

		//bool valid(id entity_id){return true;}

		template<typename T>
		void register_component(id entity_id, component<T> c){

		}
	
		template<typename return_type>
		void apply(std::function<return_type ()> func){};

		template<typename component_type, typename return_type>
		void apply_on_type(std::function<return_type ()> func){};

		void destroy(){};

	};

}