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
	w.add_property<r32>(entity.id, (r32)347);

	auto components = w.get_all_properties(0);
	while (!components.reached_end())
	{
		std::cout << (*components).utils->type << std::endl;
		++components;
	}

		
}