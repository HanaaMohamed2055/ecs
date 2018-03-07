#include "benchmark.h"

#include <cpprelude/string.h>
#include <string>

#include <ecs/helper_structures/sparse_unordered_set.h>
#include <ecs/world.h>

int main()
{
	auto int_type = utility::get_type_number<int>();
	auto u32_type = utility::get_type_number<u32>();
	auto float_type = utility::get_type_number<float>();
	auto string_type = utility::get_type_number<cpprelude::string>();
	auto string_type_again = utility::get_type_number<cpprelude::string>();
	auto std_string_type = utility::get_type_number<std::string>();

	std::cout << int_type << std::endl;
	std::cout << u32_type << std::endl;
	std::cout << float_type << std::endl;
	std::cout << string_type << std::endl;
	std::cout << string_type_again << std::endl;
	std::cout << std_string_type << std::endl;
}
