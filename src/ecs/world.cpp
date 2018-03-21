#include <ecs/world.h>

namespace ecs
{
	Entity
	World::create_entity()
	{
		return Entity(entities.insert(), this);
	}
	
	bool
	World::entity_alive(Entity entity)
	{
		return entity.world == this && entities.has(entity.entity_id);
	}
	
	//entity_components_view
	//World::get_all_entity_properties(Entity e)
	//{
	//	if (entity_alive(e))
	//	{
	//		return entity_components_view(component_pools, e.id());
	//	}
	//}

	//entity_components_view
	//World::get_all_entity_properties(ID internal_entity)
	//{
	//	/*if (entity_set.has(internal_entity))
	//	{
	//		return entity_components_view(component_pools, internal_entity.id());
	//	}*/
	//}

	//generic_component_view
	//World::get_all_world_components()
	//{
	//	return generic_component_view(component_pools);
	//}

	entity_array&
	World::get_all_world_entities()
	{
		return entities;
	}

	void
	World::kill_entity(Entity e)
	{
		if (!entity_alive(e))
			return;
		
		cpprelude::usize entity_id = e.id();
		
		// remove the entity
		entities.remove(entity_id);

		// remove the entity components
		for (auto& pool : component_pools)
		{
			if (pool.has(entity_id))
				pool.remove(entity_id);
		}
	}

	void
	World::kill_entity(ID internal_entity)
	{
		if (!entities.has(internal_entity))
			return;
		
		cpprelude::usize entity_id = internal_entity.id();

		// remove the entity
		entities.remove(entity_id);

		// remove the entity components
		for (auto& pool : component_pools)
		{
			if (pool.has(entity_id))
				pool.remove(entity_id);
		}
	}

	//void
	//World::clean_up()
	//{
	//	 //deallocate only the memory allocated by the ecs 
	//	for (auto& pool: component_pools)
	//	{
	//		for (auto component : pool.components)
	//		{
	//			if (component.managed)
	//				pool.utils->free(component.data, pool._context);
	//		}
	//		
	//	}
	//}
}
