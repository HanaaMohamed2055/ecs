#pragma once
#include <cpprelude/hash_array.h>
#include <cpprelude/string.h>

#include <ecs/elements.h>
#include <ecs/bag.h>
#include <ecs/utility.h>
#include <ecs/api.h>

namespace ecs
{
	struct World
	{
		using component_types_table = cpprelude::hash_array<cpprelude::string, cpprelude::dynamic_array<Internal_Component>>;

		bag<Entity> entity_bag;
		cpprelude::hash_array<cpprelude::u64, component_types_table> ledger;
		cpprelude::memory_context* _context;

		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context)
		{}

		API_ECS Entity
		create_entity();

		API_ECS Entity
		clone_from_Entity(Entity e);

		template<typename T>
		T&
		add_property(Entity e, T data)
		{
			cpprelude::string key(typeid(T).name(), _context);
			ledger[e.id][key].insert_back(Internal_Component());
			auto& container = ledger[e.id][key];
			auto& component = container[container.count() - 1];

			component.type = key;
			component.data = _context->alloc<T>();
			new (component.data) T(data);
			component.destroy = internal_component_dispose<T>;
			component.copy = copy<T>;

			return *(static_cast<T*>(component.data));	
		}
		
		template<typename T>
		bool
		has(Entity e)
		{
			return ((ledger.lookup(e.id) != ledger.end()) &&
				(ledger[e.id].lookup(typeid(T).name()) != ledger[e.id].end()));
		}

		template<typename T>
		bool
		remove_property(Entity e)
		{
			if (has<T>(e))
			{
				cpprelude::string type(typeid(T).name(), _context);
				
				auto& components = ledger[e.id][type];
				for (auto& c: components)
					c.destroy(c.data, _context);

				ledger[e.id].remove(type);
				return true;
			}

			// the property 
			return false;
		}

		template<typename T>
		T&
		get(Entity e)
		{	
			cpprelude::string type(typeid(T).name(), _context);
			return *(static_cast<T*>(ledger[e.id][type][0].data));
		}
				
		template<typename T>
		component_view<T>
		get_all(Entity e)
		{
			cpprelude::string type(typeid(T).name(), _context);
			auto& container = ledger[e.id][typeid(T).name()];

			cpprelude::sequential_iterator<Internal_Component> begin(container.begin());
			cpprelude::sequential_iterator<Internal_Component> end(container.end());

			return component_view<T>(begin, end);
		}

		API_ECS void
		kill_entity(Entity e, bool cleanup_mode = false);

		API_ECS void
		clean_up();
		
		~World()
		{
			clean_up();
		}
	};
}
