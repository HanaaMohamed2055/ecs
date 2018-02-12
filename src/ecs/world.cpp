#include <ecs/world.h>


namespace ecs
{
	 
	Entity World::make_entity()
	{
		cpprelude::u64 id = entity_bag.insert(Entity());
		entity_bag[id].id = id;
		entity_bag[id].world = this;
		return entity_bag[id];
	}
}