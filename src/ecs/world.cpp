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
	
	void
	World::kill_entity(cpprelude::u64 id)
	{
		//remove the entity
		entity_set.remove(id);
		
		auto entity_components = ledger[id];
		for (auto index : entity_components)
		{
			auto& component = component_set[index];
			
			auto& typed_components = type_table[component.utils->type];
			auto itr = std::find(typed_components.begin(), typed_components.end(), index);
			std::swap(typed_components[typed_components.count() - 1], *itr);
			typed_components.remove_back();
		
			component.utils->free(component.data, _context);
			component_set.remove_by_index(index);
		}

		ledger.remove(id);
	}

	//	entity_bag.remove(id);
	//	ledger.remove(id);
	//}

	//void
	//World::clean_up()
	//{
	//	 
	//}
}
