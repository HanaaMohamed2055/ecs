
#include <cpprelude/defines.h>
#include <cpprelude/string.h>
#include <cpprelude/dynamic_array.h>

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

struct Implant
{
	cpprelude::u64 id = INVALID_ID;
	World* world = nullptr;
};

int main()
{
	World w;
	auto e = w.create_entity();
	auto e1 = w.create_entity();

	for(cpprelude::u64 i = 0; i < 100000; ++i)
	{ 
		auto e = w.create_entity();
		w.add_property<u32>(e, 838);
	}
	//w.remove_property<r32>(e);
}
