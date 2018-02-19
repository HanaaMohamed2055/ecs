#pragma once
#include <cpprelude/hash_array.h>
#include <cpprelude/string.h>

#include <ecs/elements.h>
#include <ecs/bag.h>
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
			:_context(context),
			ledger(context),
			entity_bag(context)
		{}

		/**
		* creates an entity without any components/data bound to it
		*/
		API_ECS Entity
		create_entity();

		/**
		* clones the data/components bound to an entity and binds them to a new entity
		*/
		API_ECS Entity
		clone_from_Entity(Entity e);


		template<typename T>
		T&
		add_property(Entity e, T data)
		{
			Internal_Component component;
			cpprelude::string key(typeid(T).name(), _context);
			component.type = key;
			component.data = _context->alloc<T>();
			new (component.data) T(data);
			component.destroy = utility::internal_component_dispose<T>;
			component.copy = utility::copy<T>;

			ledger[e.id][key].insert_back(component);
			
			return *(static_cast<T*>(component.data));	
		}
		
		/**
		* asks whether the entity instance has the property specified
		* Must be used before get or get_all
		*/
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
				for (auto& c : components)
					c.destroy(c.data, _context);

				ledger[e.id].remove(type);
				return true;
			}

			// the property does not exist
			return false;
		}

		/**
		* gets the value of the first component it finds related to the
		* specified property
		*/
		template<typename T>
		T&
		get(Entity e)
		{	
			cpprelude::string type(typeid(T).name(), _context);
			return *(static_cast<T*>(ledger[e.id][type][0].data));
		}
				

		/**
		* gets a view of all the components related to the specified property
		*/
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

		/**
		* removes the entity with all of its components from the world
		*/
		API_ECS void
		kill_entity(Entity e, bool cleanup_mode = false);

		/**
		* kills all entities in the world
		*/
		API_ECS void
		clean_up();
		
		~World()
		{
			clean_up();
		}
	};
}
