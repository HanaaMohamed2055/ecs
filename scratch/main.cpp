#include <cpprelude/defines.h>
#include <ecs/world.h>
#include <ecs/utility.h> 


using namespace ecs;
using namespace cpprelude;

int main()
{
	World w;
	auto entity1 = w.make_entity();
	auto entity2 = w.make_entity();

	auto component1 = w.make_component<cpprelude::u32>(32, entity1.id);
	auto component2 = w.make_component<cpprelude::u32>(64, entity1.id);
	auto component3 = w.make_component<cpprelude::r32>(283.489, entity1.id);
	auto component4 = w.make_component<cpprelude::r32>(929.923, entity2.id);
	auto component5 = w.make_component<cpprelude::u32>(9, entity2.id);

	auto e1 = w.get_entity_components<cpprelude::u32>(entity1.id);
	for (auto e: e1)
		std::cout << e.get_data() << std::endl;
	
	std::cout << "-------------------------------\n";
	auto e2 = w.get_all_entity_components(entity2.id);
	for (auto e : e2)
		std::cout << e->id << std::endl;
	
	std::cout << "--------------------------------\n";
	auto e3 = w.get_all_entity_components(entity2.id);
	for (auto e : e3)
		std::cout << e->id << std::endl;

	std::cout << "--------------------------------\n";
	//w.kill_entity(entity1.id);
	//w.kill_entity(entity2.id);
	/*e1 = w.get_entity_components<cpprelude::u32>(INVALID_ID);
	e1 = w.get_world_components<cpprelude::u32>();
	for (auto e : e1)
		std::cout << e.get_data() << std::endl;*/

	type_iterator<r32> u32_it(w.component_bag.begin(), w.component_bag.count());
	type_iterator<r32> end(w.component_bag.end(), 0);

	while (u32_it!= end)
	{
		std::cout << u32_it.data() << std::endl;
		++u32_it;
	}

}

