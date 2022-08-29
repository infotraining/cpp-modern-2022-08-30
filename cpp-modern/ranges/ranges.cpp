#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <ranges>
#include <string_view>

#include "catch.hpp"

template <typename T> 
void print(const T&& rng, std::string_view desc)
{
	std::cout << desc << ": [ ";
	for(const auto& item : rng)
		std::cout << item << " ";
	std::cout << "]\n";
}

TEST_CASE("ranges")
{
	std::vector vec = {1, 4, 6, 34, 567, 87, 23, 343, 665, 42, 12, 7, 5 };
}