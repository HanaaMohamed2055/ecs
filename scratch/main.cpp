#include <cpprelude/defines.h>
#include <ecs/world.h>


using namespace ecs;
using namespace cpprelude;

int main()
{
	World w;
	Entity e = w.create_entity();
	u32& length = w.add_property<u32>(e, 100);
	w.add_property<u32>(e, 200);
	w.add_property<u32>(e, 201389);
	w.add_property<r32>(e, 25.8238);

	if (w.has<u32>(e))
		std::cout << w.get<u32>(e) << std::endl;
	else
		std::cout << "no";
	length = 321;
	std::cout << w.get<u32>(e) << std::endl;
	
	auto u32_view = w.get_all<u32>(e);
	while (!u32_view.end())
	{
		std::cout << u32_view.data() << std::endl;
		u32_view.next();
	}
}

