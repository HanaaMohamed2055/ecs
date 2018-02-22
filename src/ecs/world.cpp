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
	
	void
	World::kill_entity(cpprelude::u64 id)
	{
		auto itr = ledger.lookup(id);
		if (itr != ledger.end())
		{
			for (auto type = itr.value().begin(); type != itr.value().end(); ++type)
			{
				auto& components = type.value();

				/*for (Component& c : components)
					c.utils->free(c.data, _context);*/

				components.clear();
			}
		}

		entity_bag.remove(id);
		ledger.remove(id);
	}

	void
	World::clean_up()
	{
		 
	}
}
