#include <ecs/world.h>

namespace ecs
{
	Entity
	World::create_entity()
	{
		cpprelude::u64 entity_id = entity_set.insert(Entity());
		Entity& entity = entity_set[entity_id];
		entity.id = entity_id;
		entity.world = this;

		return entity;
	}
	
	view<generic_component_iterator>
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
	}

	sparse_unordered_set<Component>&
	World::get_all_world_components()
	{
		return component_set;
	}

	sparse_unordered_set<Entity>&
	World::get_all_world_entities()
	{
		return entity_set;
	}

	void
	World::kill_entity(Entity& e)
	{
		if (e.id == INVALID_ID || e.world != this)
			return;

		// remove the entity
		entity_set.remove(e.id);

		// remove the entity components
		auto entity_components = ledger[e.id];
		for (auto index: entity_components)
		{
			auto& component = component_set[index];
			
			auto& typed_components = type_table[component.utils->type];
			auto itr = std::find(typed_components.begin(), typed_components.end(), index);
			std::swap(typed_components[typed_components.count() - 1], *itr);
			typed_components.remove_back();
		
			if(component.dynamically_allocated)
				component.utils->free(component.data, _context);
		
			component_set.remove(index);
		}
		ledger.remove(e.id);

		// invalidate the entity
		e.id = INVALID_ID;
		e.world == nullptr;
	}

	void
	World::clean_up()
	{
		// deallocate only the memory allocated by the ecs 
		for (auto& component: component_set)
		{
			if (component.dynamically_allocated)
				component.utils->free(component.data, _context);
		}
	}
}
