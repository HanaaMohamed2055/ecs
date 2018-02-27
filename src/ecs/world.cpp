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
	
	//void
	//World::kill_entity(cpprelude::u64 id)
	//{
	//	auto itr = ledger.lookup(id);
	//	if (itr != ledger.end())
	//	{
	//		for (auto type = itr.value().begin(); type != itr.value().end(); ++type)
	//		{
	//			auto& components = type.value();

	//			/*for (Component& c : components)
	//				c.utils->free(c.data, _context);*/

	//			components.clear();
	//		}
	//	}

	//	entity_bag.remove(id);
	//	ledger.remove(id);
	//}

	//void
	//World::clean_up()
	//{
	//	 
	//}
}
