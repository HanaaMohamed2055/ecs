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
	//w.add_property<u32>(entity.id, (u32)2389);
	hash<u64> x;
	std::cout << x.operator() << std:: endl;
	

}