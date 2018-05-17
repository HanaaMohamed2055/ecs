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
	
	view<entity_components_iterator>
	World::get_all_entity_properties(Entity e)
	{
		if (entity_alive(e))
		{
			auto pool_end = component_pools.end();
			entity_components_iterator begin(component_pools.begin(), pool_end, e.id());
			entity_components_iterator end(pool_end, pool_end, e.id());
			
			return view<entity_components_iterator>(begin, end);
		}
	}

	view<entity_components_iterator>
	World::get_all_entity_properties(ID internal_entity)
	{
		if (entities.has(internal_entity))
		{
			auto pool_end = component_pools.end();
			entity_components_iterator begin(component_pools.begin(), pool_end, internal_entity.id());
			entity_components_iterator end(pool_end, pool_end, internal_entity.id());
			
			return view<entity_components_iterator>(begin, end);
		}
	}

	view<entity_components_iterator>
	World::get_all_entity_properties(cpprelude::usize entity_index)
	{
		auto pool_end = component_pools.end();
		entity_components_iterator begin(component_pools.begin(), pool_end, entity_index);
		entity_components_iterator end(pool_end, pool_end, entity_index);

		return view<entity_components_iterator>(begin, end);
	}


	view<generic_component_iterator>
	World::get_all_world_components()
	{
		generic_component_iterator begin(component_pools.begin(), component_pools.count());
		generic_component_iterator end(component_pools.end(), 0);
		
		return view<generic_component_iterator>(begin, end);
	}

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
