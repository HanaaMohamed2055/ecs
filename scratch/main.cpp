#include <cpprelude/defines.h>
#include <cpprelude/string.h>


#include <ecs/world.h>
#include <ecs/helper_structures/view.h>


using namespace ecs;
using namespace cpprelude;
using namespace utility;


struct Position
{
	r32 x = 0.0f;
	r32 y = 0.0f;
	r32 z = 0.0f;

	Position()
	{}

	Position(r32 i, r32 j, r32 k)
		:x(i), y(j), z(k)
	{}
};


int main()
{
	World w;

	auto e1 = w.create_entity();
	auto e2 = w.create_entity();
	auto e3 = w.create_entity();

	

	w.add_property<Position>(e1.id, 0.0f, 0.0f, 1.0f);
	w.add_property<Position>(e1.id, 1.0f, 1.0f, 1.0f);

	w.add_property<u32>(e1.id, 30);

	w.add_property<Position>(e2.id, 1.0f, 1.1f, 1.2f);
	w.add_property<u32>(e2.id, 32);
	w.add_property<r32>(e2.id, 234.867);

	w.add_property<Position>(e3.id, Position(2.0f, 3.0f, 5.0f));

	auto positions = w.get_world_components<Position>();
	for (auto& position : positions)
		position.x += 4;

	w.remove_property<Position>(e2.id);
			
	std::cout << w.get<Position>(e1.id).x << std::endl;
	positions = w.get_world_components<Position>();
	for (auto it = positions.begin(); it != positions.end(); ++it)
	{
		std::cout << it->entity_id << std::endl;
		std::cout << (*it).x << " " << (*it).y << " " << (*it).z << std::endl;
	}
}