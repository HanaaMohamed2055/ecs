#include <cpprelude/defines.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/string.h>
#include <cpprelude/hash_array.h>

#include <ecs/world.h>
#include <ecs/elements.h>
#include <ecs/utility.h>

using namespace ecs;
using namespace cpprelude;
using namespace utility;


int main()
{
	World w;
	auto entity = w.create_entity();
	w.add_property<u32>(entity.id, (u32)2389);
	w.add_property<u32>(entity.id, (u32)8983);
	w.add_property<r32>(entity.id, (u32)347);

	auto components = w.get_all<u32>(0);
	for (auto component : components)
		std::cout << *(u32*)component.data << std::endl;
}