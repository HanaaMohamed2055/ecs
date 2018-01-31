#include <cpprelude/defines.h>
#include <cpprelude/allocator.h>
#include <ecs/ecs.h>
#include <string>

using namespace ecs;

int main()
{	

	entity_component_manager em;
	auto cptr =  em.add_component<u32>(3, 34);
	std::cout << sizeof(*cptr) * 512 << std::endl;
	auto cptr2 = em.add_component<u32>(2, 123);
	auto cptr3 = em.add_component<std::string>(23, "hello");
	std::cout << sizeof(*cptr3) * 512 << std::endl;
	std::cout << cptr->data << std::endl;
	std::cout << cptr << std::endl;
	std::cout << cptr2->data << std::endl;
	std::cout << cptr2 << std::endl;
	std::cout << cptr3->data << std::endl;
	std::cout << cptr3 << std:: endl;
	return 0;
}