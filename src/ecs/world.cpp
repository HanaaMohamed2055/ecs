#include <ecs/world.h>

namespace ecs
{
	Entity
	World::create_entity()
	{
		cpprelude::u64 id = entity_bag.insert(Entity());
		entity_bag[id].id = id;
		entity_bag[id].world = this;
		return entity_bag[id];
	}

	Entity
	World::clone_from_Entity(Entity e)
	{
		auto entity = create_entity();

		auto itr = ledger.lookup(e.id);
		if (itr != ledger.end())
		{
			for (auto type_itr = itr.value().begin(); type_itr != itr.value().end(); ++type_itr)
			{
				auto& components = type_itr.value();

				for (Internal_Component c : components)
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

	void
	World::kill_entity(Entity e, bool cleanup_mode)
	{
		auto itr = ledger.lookup(e.id);
		if (itr != ledger.end())
		{
			for (auto type = itr.value().begin(); type != itr.value().end(); ++type)
			{
				auto& components = type.value();

				for (Internal_Component& c : components)
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
	World::clean_up()
	{
		for (auto entity : entity_bag)
		{
			kill_entity(entity, true);
		}
	}
}