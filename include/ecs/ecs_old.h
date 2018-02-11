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

#include "ecs/bag.h"

#define INVALID_ID (1 << 63) - 1
#define PREALLOCATED_COUNT MEGABYTES(1)

using namespace cpprelude;

namespace ecs 
{
	// //tick function
	using entity_id = u64;
	using component_id = u64;

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
		using func = void(*) (void*);
		void* _data;
		usize position;
		func destroy;

		base_component():
			_eid(INVALID_ID), _cid(INVALID_ID)
		{}

		base_component(component_id cid):
			_eid(INVALID_ID), _cid(cid)
		{}
		
		base_component(component_id cid, string name):
			_eid(INVALID_ID), _cid(cid), _name(name)
		{}

		~base_component()
		{
			destroy(_data);
		}
	};

	template<typename T>
	void dispose(void* d)
	{
		T* data = static_cast<T*>(d);
		data->~T();
		d = nullptr;
	}

	template<typename T>
	struct component: base_component
	{
		component():
			base_component()
		{}

		component(component_id cid):
			base_component(cid)
		{}

		component(component_id cid, T component_data):
			base_component(cid)
		{
			_data = new T(component_data);
		}

		component(T* component_data):
		{
			_data = component_data;
		}

		component(component_id cid, T* component_data):
			base_component(cid)
		{
			_data = component_data;
		}
		
		component(component_id cid, string name, T* component_data):
			base_component(cid, name) 
		{
			_data = component_data;
		}

		component(const component& c):
			base_component(c._cid, c._name)
		{
			_data = c._data;
		}

		component(component&& c) :
			base_component(c._cid, c._name)
		{
			_data = c._data;
		}

		T get_data()
		{
			return *(static_cast<T*>(_data));
		}
	};


	struct entity_component_manager
	{
	
		using component_types_table = hash_array<string, dynamic_array<usize>>;
		using deleted_component_ids = stack_array<component_id>;

		//hash_array<string, *this should be the allocator itself so maybe pool_allocator or arena_t whatever> data_types_map;
		//instead of void we may use memory_context*
		hash_array <string, std::pair<slice<byte>, usize>> data_types_map;
		hash_array <string, dynamic_array<usize>> components_table;
		hash_array <entity_id, component_types_table> entities_components_table;
		hash_array <entity_id, deleted_component_ids> ids_for_reuse;

		bag<entity> entity_bag;
		bag<base_component*> component_bag;
		bag<slice<byte>> components_inventory;

		memory_context* _context;
		
		entity_component_manager(memory_context* context = platform->global_memory)
			:_context(context)
		{}
		
		entity_id make_entity()
		{
			entity_id id = entity_bag.insert(entity());
			entity_bag[id]._eid = id;
			return id;
		}


		void kill_entity(entity_id eid, bool deep = true)
		{
			// if deep is true, all components bound to the entity will be deleted not just unbound
			if (!entity_bag[eid].is_valid) return;
			
			entity_bag[eid].remove();
			entity_bag.remove(eid);
			
			// remove all components bound to an entity
			if (deep && entities_components_table.lookup(eid) != entities_components_table.end())
			{
				auto& table = entities_components_table[eid];
				for (auto type = table.begin(); type != table.end(); ++type)
				{
					dynamic_array<usize>& container = type.value();
					for (auto index : container)
					{
						component_bag.remove(index);
						delete component_bag[index];
						component_bag[index] = nullptr;
					}
					container.clear();
				}
				table.clear();
			}
		}

		// this function needs to be visited to handle the case of reusing the place of an old component

		template<typename T>
		component<T>* make_component(T data, string name = "", bool SoA = true)
		{
			auto key = typeid(T).name();
			//MOSTAFA
			//auto t = data_types_map[key]->alloc<T>();
			//MOSTAFA

			//SoA - structure of arrays is the default mode
			if (SoA)
			{
				auto found = data_types_map.lookup(key);
				if (found == data_types_map.end())
				{
					data_types_map[key].first = _context->alloc<T>(PREALLOCATED_COUNT).convert<byte>();
					data_types_map[key].second = 0;
				}

				// set component data
				auto data_slice = data_types_map[key].first.convert<T>();
				new (&data_slice[data_types_map[key].second]) T(data);
				T* data_ptr = static_cast<T*>(&data_slice[data_types_map[key].second]);
				++data_types_map[key].second;

				// create component, add it to the system bag and cache its type
				usize index = components_inventory.insert(_context->alloc<component<T>>().convert<byte>());
				slice<component<T>> component_slice = components_inventory.back()->convert<component<T>>();
				new (component_slice) component<T>(INVALID_ID, name, data_ptr);
				component<T>* component_ptr = *(&component_slice);
				component_ptr->position = index;
				component_bag.insert(static_cast<base_component*>(component_ptr));
				components_table[key].insert_back(index);

				/////////////////old stuff//////////////////////////////////////
				//base_component* component_ptr = new component<T>(INVALID_ID, name, data_ptr);
				//component_ptr->destroy = dispose<T>;
				//usize index = component_bag.insert(component_ptr);
				//component_ptr->position = index;
				//components_table[key].insert_back(index);
				////////////////////////////////////////////////////////////////

				return component_ptr;
			}

			return nullptr;
		}

		template<typename T>
		bool entity_id eid, component<T>* c)
		{
			// for now, a component should be bound only to one entity
			if (c->_eid != INVALID_ID) 
				return false;
			c->_eid = eid;
			
			if (ids_for_reuse.lookup(eid) != ids_for_reuse.end())
			{
				c->_cid = ids_for_reuse[eid].top();
				ids_for_reuse[eid].pop();
			}
			else
				c->_cid = entity_bag[eid].component_count;

			++entity_bag[eid].component_count;
	
			entities_components_table[eid][typeid(T).name()].insert_back(c->position);
			return true;
		}


		template<typename T>
		bool unbind_component(component<T>* c)
		{
			if (c->_eid == INVALID_ID) 
				return false;
		
			auto& container = entities_components_table[c->_eid][typeid(T).name()];
			
			// remove the index of the component from entity_component_table
			auto index = std::find(container.begin(), container.end(), c->position) - container.begin();
			std::swap(container[index], container[container.count() - 1]);
			container.remove_back(1);
	
			--entity_bag[c->_eid].component_count;
			ids_for_reuse[c->_eid].push(c->_cid);
			c->_eid = INVALID_ID;
			c->_cid = INVALID_ID;

			return true;
		}


		dynamic_array<base_component*> get_all_components( )
		{
			// if invalid id is supplied, no components will be returned
			dynamic_array<base_component*> components;

			if (entities_components_table.lookup(id) != entities_components_table.end())
			{
				auto& components_table = entities_components_table[id];
				for (auto it = components_table.begin(); it != components_table.end(); ++it)
				{
					auto& container = it.value();
					for (auto index : container)
					{
						auto ptr = component_bag[index];
						components.insert_back(ptr);
					}
				}
			}

			return components;
		}

		template<typename T>
		dynamic_array<component<T>*> get_components_by_type(entity_id id = INVALID_ID)
		{
			// if no id or invalid ID was supplied, all components of the type
			// specified will be returned
			dynamic_array<component<T>*> components;
			auto key = typeid(T).name();
					
			if (id == INVALID_ID && components_table.lookup(key) != components_table.end())
			{
				auto& container = components_table[key];
				for (auto index : container)
				{
					auto c_ptr = static_cast<component<T>*>(component_bag[index]);
					components.insert_back(c_ptr);
				}
			}
			else if (entities_components_table.lookup(id) != entities_components_table.end()
				&& entities_components_table[id].lookup(key) != entities_components_table[id].end())
			{
				auto& container = entities_components_table[id][key];
				for (auto index : container)
				{
					auto c_ptr = static_cast<component<T>*>(component_bag[index]);
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
		
		template<typename return_type>
		void apply(std::function<return_type ()> func)
		{}

		template<typename component_type, typename return_type>
		void apply_on_type(std::function<return_type ()> func)
		{}

		~entity_component_manager()
		{
			// delete allocated components
			for (auto it= data_types_map.begin(); it != data_types_map.end(); ++it)
			{
				auto slice = it.value().first;
				_context->free(slice);
			}

			/*for (auto component_ptr : component_bag)
				delete component_ptr;*/
		}
	};
}