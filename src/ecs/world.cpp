#include <ecs/world.h>

namespace ecs
{
	Entity
	World::create_entity()
	{
		return Entity(entity_set.insert_one_more(), this);
	}
	
	bool
	World::entity_alive(Entity entity)
	{
		return entity.world == this && entity_set.has(entity);
	}
	
	/*view<generic_component_iterator>
	World::get_all_entity_properties(Entity e)
	{
		if (e.id != INVALID_ID && e.world == this)
		{
			auto& components = ledger[e.id];
			generic_component_iterator begin(&component_set, components.begin());
			generic_component_iterator end(&component_set, components.end());
			return view<generic_component_iterator>(begin, end);
		}

		return view<generic_component_iterator>();
	}*/

	/*sparse_unordered_set<Component>&
	World::get_all_world_components()
	{
		return component_set;
	}*/

	/*sparse_unordered_set<Entity>&
	World::get_all_world_entities()
	{
		return entity_set;
	}*/

	void
	World::kill_entity(Entity e)
	{
		if (!entity_alive(e))
			return;
		
		cpprelude::usize entity_id = e.id();
		
		// remove the entity
		entity_set.remove(entity_id);

		// remove the entity components
		for (auto& pool : component_types)
		{
			auto& components = pool.components;
			for (auto& component : components)
			{
				if (component.first.entity_id == entity_id)
				{
					if (component.first.managed)
						pool.utils->free(component.first.data, pool._context);

					pool.components.remove(component.second);
					break;
				}
			}
		}
	}

	//void
	//World::clean_up()
	//{
	//	 //deallocate only the memory allocated by the ecs 
	//	for (auto& component: component_set)
	//	{
	//		if (component.dynamically_allocated)
	//			component.utils->free(component.data, _context);
	//	}
	//}
}
