#include <ecs/world.h>

namespace ecs
{
	Entity
	World::make_entity()
	{
		cpprelude::u64 id = entity_bag.insert(Entity());
		entity_bag[id].id = id;
		entity_bag[id].world = this;
		return entity_bag[id];
	}


	void
	World::kill_entity(cpprelude::u64 entity_id)
	{
		auto& component_ids = entity_components[entity_id];
		for (cpprelude::u64 cid : component_ids)
		{
			remove_component(cid, false);
		}
		component_ids.clear();
		entity_bag.remove(entity_id);
	}


	cpprelude::dynamic_array<Internal_Component*>
	World::get_all_entity_components(cpprelude::u64 entity_id)
	{
		cpprelude::dynamic_array<Internal_Component*> components;

		if (entity_id != INVALID_ID)
		{
			auto& container = entity_components[entity_id];
			for (auto index : container)
				components.insert_back(&component_bag[index]);
		}

		return components;
	}


	void
	World::remove_component(cpprelude::u64 component_id, bool unbind_from_entity)
	{
		auto& container = component_types[component_bag[component_id].type];

		component_bag[component_id].destroy(_context);
		component_bag.remove(component_id);

		auto itr = std::find(container.begin(), container.end(), component_id);
		if (itr != container.end())
		{
			auto index = itr - container.begin();
			std::swap(container[index], container[container.count() - 1]);
			container.remove_back();
		}
	}


	void
	World::clean_up()
	{
		// here, we should iterate on remaining valid components and destroy them 
		// free the memory allocated that will not be freed by ~bag()
		for (auto& component : component_bag)
			component.destroy(_context);
	}
}