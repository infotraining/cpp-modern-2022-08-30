#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include "catch.hpp"

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
}

TEST_CASE("Before C++17")
{
	using namespace BeforeCpp17;

	REQUIRE(sum(1, 2, 3, 4, 5) == 15);
}