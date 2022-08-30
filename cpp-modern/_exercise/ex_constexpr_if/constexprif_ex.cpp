#include "catch.hpp"

#include <iostream>
#include <list>
#include <vector>

using namespace std;

struct FastPath
{
};

struct SlowPath
{
};



TEST_CASE("constexpr-if with iterator categories")
{
    SECTION("random_access_iterator")
    {
        vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        auto result = advance_it(it, 3);

        static_assert(std::is_same_v<decltype(result), FastPath>);

        REQUIRE(*it == 4);
    }

    SECTION("input_iterator")
    {
        list<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        auto result = advance_it(it, 3);

        static_assert(std::is_same_v<decltype(result), SlowPath>);

        REQUIRE(*it == 4);
    }
}