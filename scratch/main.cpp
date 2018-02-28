#include <cpprelude/defines.h>
#include <cpprelude/string.h>


#include <ecs/world.h>
#include <ecs/helper_structures/view.h>


using namespace ecs;
using namespace cpprelude;
using namespace utility;


int main()
{
	World w;
	auto entity = w.create_entity();
	w.add_property<u32>(entity.id, (u32)2389);
	w.add_property<u32>(entity.id, (u32)8983);
	r32 data = 34.7;
	w.add_property<r32>(entity.id, &data);
	w.add_property<r32>(entity.id, (r32)34.740);
	w.add_property<u32>(entity.id, (u32)347930);

	auto components = w.get_entity_properties<u32>(0);
	for (auto& component : components)
			++component;

	auto components2 = w.get_world_components<u32>();
	for (auto component : components2)
		std::cout << component << std::endl;

	auto all_components = w.get_all_entity_properties(0);
	for (auto component : all_components)
		std::cout << component.utils->type << std::endl;
		
}