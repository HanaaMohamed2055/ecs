#pragma once
#include <cpprelude/hash_array.h>
#include <cpprelude/string.h>

#include <ecs/elements.h>
#include <ecs/bag.h>
#include <ecs/utility.h>
#include <ecs/api.h>

namespace ecs
{
	API_ECS bool dummy();
	

	struct World
	{
		using component_types_table = cpprelude::hash_array<cpprelude::string, cpprelude::dynamic_array<Internal_Component>>;

		template<typename T>
		using view = std::pair<type_iterator<T>, type_iterator<T>>;

		bag<Entity> entity_bag;
		cpprelude::hash_array<cpprelude::u64, component_types_table> ledger;
		cpprelude::memory_context* _context;

		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context)
		{}

		Entity create_entity()
		{
			cpprelude::u64 id = entity_bag.insert(Entity());
			entity_bag[id].id = id;
			entity_bag[id].world = this;
			return entity_bag[id];
		}

		template<typename T>
		T& add_property(Entity e, T data)
		{
			cpprelude::string key(typeid(T).name(), _context);
			ledger[e.id][key].insert_back(Internal_Component());
			auto& container = ledger[e.id][key];
			auto& component = container[container.count() - 1];

			component.type = key;
			component.data = _context->alloc<T>();
			new (component.data) T(data);
			component.destroy = internal_component_dispose<T>;

			return *(static_cast<T*>(component.data));	
		}
		
		template<typename T>
		bool has(Entity e)
		{
			return ((ledger.lookup(e.id) != ledger.end()) &&
				(ledger[e.id].lookup(typeid(T).name()) != ledger[e.id].end()));
		}

		template<typename T>
		T& get(Entity e)
		{	
			assert(has<T>(e) == true);
			return *(static_cast<T*>(ledger[e.id][typeid(T).name()][0].data));
		}

		template<typename T>
		view<T> get_all(Entity e)
		{
			assert(has<T>(e) == true);
			auto& container = ledger[e.id][typeid(T).name()];
			
			return view<T>(type_iterator<T> begin(container.begin(), container.count()),
			               type_iterator<T> end(container.end(), 0));	

		}

		
	};
}
