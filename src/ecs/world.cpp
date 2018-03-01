#include <ecs/world.h>

namespace ecs
{
	Entity
	World::create_entity()
	{
		cpprelude::u64 entity_id = entity_set.insert(Entity());
		cpprelude::usize index = entity_set.count() - 1;
		entity_set[index].id = entity_id;
		entity_set[index].world = this;

		return entity_set[index];
	}
	
	view<generic_component_iterator>
	World::get_all_entity_properties(cpprelude::u64 entity_id)
	{
		auto& components = ledger[entity_id];
		generic_component_iterator begin(component_set.begin(), components.begin());
		generic_component_iterator end(component_set.end(), components.end());
		return view<generic_component_iterator>(begin, end);
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
	World::kill_entity(cpprelude::u64 id)
	{
		// remove the entity
		entity_set.remove(id);

		// remove the entity components
		auto entity_components = ledger[id];
		for (auto index : entity_components)
		{
			auto& component = component_set[index];
			
			auto& typed_components = type_table[component.utils->type];
			auto itr = std::find(typed_components.begin(), typed_components.end(), index);
			std::swap(typed_components[typed_components.count() - 1], *itr);
			typed_components.remove_back();
		
			if(component.dynamically_allocated)
				component.utils->free(component.data, _context);
		
			component_set.remove_at(index);
		}

		ledger.remove(id);
	}

	void
	World::clean_up()
	{
		// deallocate only the memory allocated by the ecs 
		for (auto& component : component_set)
		{
			if (component.dynamically_allocated)
				component.utils->free(component.data, _context);
		}
	}
}
