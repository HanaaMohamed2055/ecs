#include <cpprelude/defines.h>
#include <cpprelude/allocator.h>
#include <ecs/ecs.h>
#include <string>

using namespace ecs;

int main()
{	
	entity_component_manager em;
	for (int i = 0; i < PREALLOCATED_COUNT * 2; ++i)
	{
		em.make_component((u32)i + 1);
	}
	auto result = em.get_components_by_type<u32>();
	for (auto& r : result)
		std::cout << r->_data << std::endl;
	return 0;
}

//auto cptr = em.make_component<u32>(23, "my_first_component");
//auto cptr2 = em.make_component<u32>(34, "my_second_component");
//auto cptr3 = em.make_component<r64>(100.484, "my third component");
//auto cptr4 = em.make_component<r64>(20239.894731, "my fourth component");
//std::cout << *(cptr->_data) << std::endl;
//std::cout << *(cptr2->_data) << std::endl;
//std::cout << *(cptr3->_data) << std::endl;
//std::cout << *(cptr4->_data) << std::endl;
//auto result = em.get_components_by_type<u32>();
//auto result2 = em.get_components_by_type<r64>();
//for (auto& r : result)
//std::cout << r << std::endl;
//for (auto& r : result2)
//std::cout << r << std::endl;