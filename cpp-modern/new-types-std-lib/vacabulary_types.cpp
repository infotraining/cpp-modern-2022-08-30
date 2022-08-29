#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <string_view>
#include <array>
#include <optional>
#include <variant>
#include <any>
#include <set>

#include "catch.hpp"

using namespace std::literals;

template <typename TCollection> 
void print(const TCollection& coll, const std::string& desc)
{
	std::cout << desc << ": [ ";
	for(const auto& item : coll)
		std::cout << item << " ";
	std::cout << "]\n";
}