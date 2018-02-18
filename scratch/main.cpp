#include <cpprelude/defines.h>
#include <ecs/world.h>


using namespace ecs;
using namespace cpprelude;



int main()
{
	World w;
	Entity e = w.create_entity();
	Entity e1 = w.create_entity();
	u32& length = w.add_property<u32>(e, 100);
	w.add_property<u32>(e, 200);
	w.add_property<u32>(e, 201389);
	w.add_property<r32>(e, 25.8238);
	w.add_property<u32>(e1, 25);
	w.add_property<u32>(e1, 235);
	w.add_property<r32>(e1, 25.8238);
	w.add_property<cpprelude::string>(e1, "Hello");
	
	if (w.has<u32>(e))
	std::cout << w.get<u32>(e) << std::endl;
	else
	std::cout << "no";
	length = 321;
	std::cout << w.get<u32>(e) << std::endl;
	std::cout << "------------------------------\n";
	auto u32_view = w.get_all<u32>(e);
	while (!u32_view.end())
	{
		u32& prop = u32_view.data();
		std::cout << prop << std::endl;
		prop += 1;
		u32_view.next();
	}
	std::cout << "--------------------------------\n";
	auto view = w.get_all<u32>(e);
	while (!view.end())
	{
		std::cout << view.data() << std::endl;
		view.next();
	}
	
	w.remove_property<u32>(e);
	if (w.has<u32>(e))
	std::cout << "not successful" << std::endl;
	else
	std::cout << "successful\n";
	std::cout << w.get<r32>(e) << std::endl;
	
	std::cout << "---------------------------------\n";
	u32_view = w.get_all<u32>(e1);
	while (!u32_view.end())
	{
		std::cout << u32_view.data() << std::endl;
		u32_view.next();
	}
	std::cout << "-----------------------------------\n";
	Entity e2 = w.clone_from_Entity(e1);
	auto e2_view = w.get_all<u32>(e2);
	while (!e2_view.end())
	{
		std::cout << e2_view.data() << std::endl;
		e2_view.next();
	}
	std::cout << "------------------------------------\n";
	w.kill_entity(e1);
	w.add_property<u32>(e2, 84756);
	e2_view = w.get_all<u32>(e2);
	while (!e2_view.end())
	{
		std::cout << e2_view.data() << std::endl;
		e2_view.next();
	}
}