#include <cpprelude/defines.h>
#include <ecs/ecs.h>


using namespace ecs;

int main()
{
	World w;
	auto entity1 = w.make_entity();
	auto entity2 = w.make_entity();

	auto component1 = w.make_component<u32>(32, entity1.id);
	auto component2 = w.make_component<u32>(64, entity1.id);
	auto component3 = w.make_component<r32>(283.489, entity1.id);
	auto component4 = w.make_component<r32>(929.923, entity2.id);
	auto component5 = w.make_component<u32>(9, entity2.id);

	auto e1 = w.get_components_by_type<u32>(entity1.id);
	for (auto e: e1)
		std::cout << e.get_data() << std::endl;
	
	std::cout << "-------------------------------\n";
	auto e2 = w.get_all_components(entity2.id);
	for (auto e : e2)
		std::cout << e->id << std::endl;

	std::cout << "--------------------------------\n";
	w.kill_entity(entity1.id);
	w.kill_entity(entity2.id);
	e1 = w.get_components_by_type<u32>();
	for (auto e : e1)
		std::cout << e.get_data() << std::endl;
}

