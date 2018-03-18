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
		return entity.world == this && entity_set.has(entity.id());
	}
	
	entity_components_view
	World::get_all_entity_properties(Entity e)
	{
		if (entity_alive(e))
		{
			return entity_components_view(component_types, e.id());
		}
	}

	generic_component_view
	World::get_all_world_components()
	{
		return generic_component_view(component_types);
	}

	sparse_unordered_set<Entity>&
	World::get_all_world_entities()
	{
		return entity_set;
	}

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
			if (components.has(entity_id))
			{
				auto& component = components[entity_id];
				
				if (component.managed)
					pool.utils->free(component.data, pool._context);
			
				components.remove(entity_id);
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
