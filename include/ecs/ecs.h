# pragma once

#include <functional>
#include <utility>
#include <new>

#include "cpprelude/defines.h"
#include "cpprelude/dynamic_array.h"
#include "cpprelude/queue_array.h"
#include "cpprelude/memory.h"
#include "cpprelude/platform.h"
#include "cpprelude/hash_array.h"
#include "cpprelude/string.h"

#define INVALID_ID 0
#define PREALLOCATED_COUNT 512

using namespace cpprelude;

namespace ecs 
{
	using entity_id = u32;
	using component_id = u32;


	// //tick function

	struct world
	{
		world* prev;
	};

	struct entity
	{
		entity_id _eid;
		bool is_valid;
		u32 component_count;
		world* World;

		entity():
			_eid(INVALID_ID), is_valid(true), component_count(0), World(nullptr)
		{}

		entity(entity_id eid):
			_eid(eid), is_valid(true), component_count(0), World(nullptr)
		{}

		entity(entity_id eid, world* w):
			_eid(eid), is_valid(true), component_count(0), World(w)
		{}

		void remove()
		{
			is_valid = false;
		}
	};


	struct base_component
	{
		entity_id _eid;
		component_id _cid;
		string _name;

		base_component():
			_eid(INVALID_ID), _cid(INVALID_ID)
		{}

		base_component(component_id cid):
			_eid(INVALID_ID), _cid(cid)
		{}
		
		base_component(component_id cid, string name):
			_eid(INVALID_ID), _cid(cid), _name(name)
		{}
			
	};

	template<typename T>
	struct component: base_component
	{
		T* _data;

		component():
			base_component()
		{}

		component(component_id cid):
			base_component(cid)
		{}

		component(component_id cid, T component_data):
			base_component(cid), _data(new T(component_data))
		{}

		component(T* component_data):
			data(component_data)

		component(component_id cid, T* component_data):
			base_component(cid), _data(component_data)
		{}
		
		component(component_id cid, string name, T* component_data):
			base_component(cid, name), _data(component_data)
		{}

		component(const component& c):
			base_component(c._cid, c._name), _data(new T(c.data))
		{}

		component(component&& c) :
			base_component(c._cid, c._name), _data(new T(std::move(c._data)))
		{}
	
	};


	struct entity_component_manager
	{
	
		using component_types_table = hash_array<string, dynamic_array<base_component*>>;
		
		hash_array <string, std::pair<dynamic_array<void*>, int>> data_types_map;
		hash_array <string, dynamic_array<base_component*>> components_table;
		hash_array <entity_id, component_types_table> entities_components_table;

		dynamic_array<entity> entity_index;

		queue_array<entity_id> free_places;

		template<typename T>
		entity* make_entity()
		{
			entity_id new_id;
			if (free_places.empty())
			{
				new_id = free_places.front();
				free_places.dequeue();
			}
			else
				new_id = entity_index.count() + 1;

		}

		template<typename T>
		component<T>* make_component(T data, string name = "", bool SoA = true)
		{
			auto key = typeid(T).name();
			//SoA is the default mode, preallocation is 
			if (SoA)
			{
				auto found = data_types_map.lookup(key);
				if (found == data_types_map.end() || (found != data_types_map.end() && found.value().second == PREALLOCATED_COUNT - 1))
				{
					T* new_type_ptr = new T[PREALLOCATED_COUNT];
					data_types_map[key].first.insert_back((void*)new_type_ptr);
					data_types_map[key].second = 0;
				}
				T* data_ptr = reinterpret_cast<T*>(*(data_types_map[key].first.back()));
				data_ptr[data_types_map[key].second] = std::move(data);
				base_component* component_ptr = new component<T>(INVALID_ID, name, &data_ptr[data_types_map[key].second]);
				++data_types_map[key].second;
				components_table[key].insert_back(component_ptr);
				return static_cast<component<T>*>(component_ptr);
			}

			return nullptr;
		}

		dynamic_array<base_component*> get_all_components(entity_id id)
		{
			// if invalid id is supplied, no components will be returned
			dynamic_array<base_component*> components;

			if (entities_components_table.lookup(id) != entities_components_table.end())
			{
				auto& components_table = entities_components_table[id];
				for (auto it = components_table.begin(); it != components_table.end(); ++it)
				{
					auto& container = it.value();
					for (auto ptr : container)
						components.insert_back(ptr);
				}
			}

			return components;
		}

		template<typename T>
		dynamic_array<component<T>*> get_components_by_type(entity_id id = INVALID_ID)
		{
			//if no id or invalid ID was supplied, all components of the type
			//specified will be returned
			dynamic_array<component<T>*> components;
			auto key = typeid(T).name();

			if (id == INVALID_ID && components_table.lookup(key) != components_table.end())
			{
				auto& container = components_table[key];
				for (auto ptr: container)
				{
					auto c_ptr = static_cast<component<T>*>(ptr);
					components.insert_back(c_ptr);
				}
			}

			else if (entities_components_table.lookup(id) != entities_components_table.end()
					&& entities_components_table[id].lookup(key) != entities_components_table[id].end())
			{
				auto& container = entities_components_table[id][key];
				for (auto ptr: container)
				{
					auto c_ptr = static_cast<component<T>*>(ptr);
					components.insert_back(c_ptr);
				}
			}

			return components;
		}

		template<typename T>
		bool process(component<T> & data_component)
		{
			//this functin should apply actions stored for queued components in a map of job queues
			//each component in the hash table will have a queue of jobs
			//process will always be called for each component in the queue at the next tick.
			return true;
		}

		void kill_entity(entity_id eid)
		{

			// this should remove the entity from the entities index
			// and remove all components binded to it
		}

		~entity_component_manager()
		{
			
		}
	};

	// old stuff //
	// struct entity
	// {	
	// 	entity_id eid;
	// 	bool  is_valid;
	// 	u32 component_count; // every entity should register to no more than 256 components

	// 	entity()
	// 	{
	// 		eid = 0;
	// 		is_valid = true;
	// 		component_count = 0;
	// 	}

	// 	entity(entity_id id)
	// 	{
	// 		eid = id;
	// 		is_valid = true;
	// 		component_count = 0;
	// 	}

	// 	//this will act as a signal for the entity manager 
	// 	//to destroy entity
	// 	//deferred update??
	// 	void remove(bool immediate = false) 
	// 	{
	// 		is_valid = false;
	// 	}
	// };	
	

	// /** A component is a wrapper for whatever kind of data.
	// * it can only be registered to by a single entity
	// * An entity can register to any component ( Dynamic aggregation )
	// */
	// template<typename T>
	// struct component
	// {
	// 	using component_type = T;
		
	// 	// component id must be unique within each entity		
	// 	component_id cid;
	// 	entity_id eid;

	// 	component_type data;

	// 	component()
	// 		:cid(INVALID_ID), eid(INVALID_ID)
	// 	{}

	// 	component(component_type* component_data)
	// 		:cid(INVALID_ID), eid(INVALID_ID), data(*component_data)
	// 	{}

	// 	component(component_id id, component_type* component_data)
	// 		:cid(id), eid(INVALID_ID), data(*component_data)
	// 	{}


	// 	component(component_id id, component_type component_data)
	// 		:cid(id), eid(INVALID_ID), data(component_data)
	// 	{}

	// 	// component(component&&) noexcept = default;
	// };



	// /**The entity manager (so far) is responsible for creating, destroying entities,
	// *reusing places of destroyed entities and applying a certain action
	// *over all entities or entities registered to certain type(s) of components
	// */
	// //this should take memory_context whatever which allocator has given  it -> resource manager
	// struct entity_component_manager
	// {
	// 	dynamic_array<entity> entities_index;
	// 	queue_array<entity_id> free_places;
	// 	hash_array<entity_id, dynamic_array<void*>> entities_components;
	// 	hash_array <cpprelude::string, std::pair<dynamic_array<void*>, usize>> types_components_map;
	// 	memory_context* _context;

	// 	entity_component_manager(memory_context* context = platform->global_memory)
	// 		:_context(context)
	// 	{}

	// 	entity* create()
	// 	{
	// 		entity_id new_id;

	// 		if(free_places.empty())
	// 			new_id = entities_index.count() + 1;
	// 		else
	// 		{	
	// 			new_id = free_places.front();
	// 			//this is a bool, decide later whether to check or not
	// 			free_places.dequeue();
	// 		}	

	// 		entities_index.emplace_back(new_id);
	// 		return &entities_index[entities_index.count() - 1];
	// 	}	

		
	// 	// Array of structures is enabled by default
	// 	template<typename T>
	// 	bool bind(entity* e, component<T>& c)
	// 	{
	// 		if (c.eid != INVALID_ID) 
	// 			return false;
	// 		c.eid = e->eid;
	// 		c.cid = ++(e->component_count);  
	// 		entities_components[e->eid].insert_back(&c);
	// 		return true;
	// 	}

	// 	//default behaviour is structure of arrays
	// 	template<typename T>
	// 	component<T>* make_component(component_id id, T data, bool aos = false, bool prealocation = true)
	// 	{
	// 		auto key = typeid(T).name();

	// 		if (!aos)
	// 		{

	// 			if (types_components_map.lookup(key) == types_components_map.end() 
	// 				|| (types_components_map.lookup(key) != types_components_map.end() && types_components_map[key].second == PREALLOCATED_COUNT - 1))
	// 			{

	// 				//auto slice_ptr = new slice<component<T>>(_context->alloc<component<T>>(static_cast<usize>(PREALLOCATED_COUNT)));
	// 				void* new_type_ptr = new component<T>[PREALLOCATED_COUNT];
	// 				types_components_map[key].first.insert_back(new_type_ptr);
	// 				types_components_map[key].second = 0;
	// 			}

	// 			component<T>* data_ptr = reinterpret_cast<component<T>*>(*(types_components_map[key].first.back()));
	// 			new (&data_ptr[types_components_map[key].second++]) component<T>(id, data); 
	// 			return &data_ptr[types_components_map[key].second - 1];
	// 		}
	// 		return nullptr;
	// 	}

	// 	template<typename T>
	// 	dynamic_array<component<T>*> get_components_by_type(entity_id id = INVALID_ID)
	// 	{
	// 		auto key = typeid(T).name();
	// 		dynamic_array<component<T>*> result;
	// 		if (id == INVALID_ID && types_components_map.lookup(key) != types_components_map.end())
	// 		{
	// 			auto component_ptrs = types_components_map[key].first;
	// 			size_t size = component_ptrs.count();
	// 			size_t index_count = PREALLOCATED_COUNT;
	// 			size_t last_index_count = types_components_map[key].second;
	// 			for (size_t i = 0; i < size; ++i)
	// 			{
	// 				if (i == size - 1)
	// 					index_count = last_index_count;
	// 				for (int index = 0; index < index_count; ++index)
	// 				{
	// 					auto c_ptr = reinterpret_cast<component<T>*>(component_ptrs[index]);
	// 					result.insert_back(c_ptr);
	// 				}
	// 			}
	// 		}
	// 		return result;
	// 	}
				
	// 	template<typename return_type>
	// 	void apply(std::function<return_type ()> func)
	// 	{}

	// 	template<typename component_type, typename return_type>
	// 	void apply_on_type(std::function<return_type ()> func)
	// 	{}

	// 	void destroy()
	// 	{
	// 		for(size_t i = 0; i < entities_index.count(); ++i)
	// 		{
	// 			if(!entities_index[i].is_valid)
	// 			{
	// 				free_places.enqueue(entities_index[i].eid);	
	// 				std::swap(entities_index[i], entities_index[entities_index.count() - 1]);
	// 				entities_index.remove_back(1);
	// 			}
	// 		}
	// 	}

	// 	void free_all()
	// 	{
	// 		for (auto it = types_components_map.begin(); it != types_components_map.end(); ++it)
	// 		{
				
	// 		}
	// 	}
	//
}