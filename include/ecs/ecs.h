#pragma once

#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>
#include <cpprelude/string.h>
#include <cpprelude/hash_array.h>
#include <cpprelude/dynamic_array.h>

#include <ecs/bag.h> 
using namespace cpprelude;

namespace ecs
{
	constexpr u64 INVALID_ID = -1UL;

	struct World; 	

	struct Entity
	{
		cpprelude::u64 id = INVALID_ID;
		World* world = nullptr;	
	};

	struct Internal_Component
	{
		using free_func = void(*) (void*);

		cpprelude::u64 id = INVALID_ID;
		cpprelude::u64 entity_id = INVALID_ID;
		void* _data = nullptr;
		World* world = nullptr;
		free_func _destroy = nullptr;

		void destroy()
		{
			_destroy(_data);
		}		

		~Internal_Component()
		{
			if (_data == nullptr) return;
			destroy();
		}

	};

	template<typename T>
	void internal_component_dispose(void* d)
	{
		T* data = (T*)(d);
		data->~T();
	}

	template<typename T>
	struct Component
	{
		Internal_Component* component;
		
		Component(Internal_Component* c)
			:component(c)
		{}

		T get_data()
		{
			return *(static_cast<T*>(component->_data));
		}
	};

	struct World
	{
		bag<Entity> entity_bag;
		bag<Internal_Component> component_bag;
		memory_context* _context;

		hash_array<u64, dynamic_array<u64>> entity_components;
		hash_array<cpprelude::string, dynamic_array<u64>> component_types;

		World(memory_context* context = platform->global_memory)
			:_context(context)
		{}
		

		Entity make_entity()
		{
			u64 id = entity_bag.insert(Entity());
			entity_bag[id].id = id;
			entity_bag[id].world = this;
			return entity_bag[id];
		}


		template<typename T>
		Component<T> make_component(T data, u64 entity_id)
		{
			u64 id = component_bag.insert(Internal_Component());

			// initialize component
			auto& component = component_bag[id];
			component.id = id;
			component.entity_id = entity_id;
			component._data = _context->alloc<T>();
			new (component._data) T(data);
			component._destroy = internal_component_dispose<T>;
			component.world = this;
			
			// bind the component to the entity and cache its type
			entity_components[entity_id].insert_back(id);
			cpprelude::string key = typeid(T).name();
			component_types[key].insert_back(id);

			return &component;
		}


		//if no ID or invalid ID was supplied, all components of the type specified
		//will be returned
		template<typename T>
		dynamic_array<Component<T>> get_components_by_type(u64 entity_id = INVALID_ID)
		{
			dynamic_array<Component<T>> components; 
			cpprelude:string key = typeid(T).name();

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


		dynamic_array<Internal_Component*> get_all_components(u64 entity_id)
		{
			dynamic_array<Internal_Component*> components;
			
			if(entity_id != INVALID_ID)
			{
				auto& container = entity_components[entity_id];
				for (auto index : container)
					components.insert_back(&component_bag[index]);
			}

			return components;
		}
	};

}