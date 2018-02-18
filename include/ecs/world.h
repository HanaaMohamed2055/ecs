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

		bag<Entity> entity_bag;
		cpprelude::hash_array<cpprelude::u64, component_types_table> ledger;
		cpprelude::memory_context* _context;

		World(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_context(context)
		{}

		Entity 
		create_entity()
		{
			cpprelude::u64 id = entity_bag.insert(Entity());
			entity_bag[id].id = id;
			entity_bag[id].world = this;
			return entity_bag[id];
		}

		Entity clone_from_Entity(Entity e)
		{
			auto entity = create_entity();
			
			auto itr = ledger.lookup(e.id);
			if(itr != ledger.end())
			{
				for (auto type_itr = itr.value().begin(); type_itr != itr.value().end(); ++type_itr)
				{
					auto& components = type_itr.value();

					for (Internal_Component c: components)
					{
						ledger[entity.id][type_itr.key()].insert_back(Internal_Component());
						auto& container = ledger[entity.id][type_itr.key()];
						auto& component = container[container.count() - 1];
						c.copy(c.data, component, _context);
					}
				}
			}

			return entity;
		}

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

		void
		kill_entity(Entity e, bool cleanup_mode = false)
		{
			auto itr = ledger.lookup(e.id);
			if (itr != ledger.end())
			{
				for (auto type = itr.value().begin(); type != itr.value().end(); ++type)
				{
					auto& components = type.value();

					for (Internal_Component& c: components)
						c.destroy(c.data, _context);

					components.clear();
				}
			}

			if (!cleanup_mode)
			{
				entity_bag.remove(e.id);
				ledger.remove(e.id);
			}
		}

		void
		clean_up()
		{
			for (auto entity: entity_bag)
			{
				kill_entity(entity, true);
			}
		}

		~World()
		{
			clean_up();
		}
	};
}
