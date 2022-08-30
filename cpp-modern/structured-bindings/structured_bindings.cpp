#include "catch.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;

namespace BeforeCpp17
{
    std::tuple<int, int, double> calc_stats(const std::vector<int>& data)
    {
        std::vector<int>::const_iterator min_pos, max_pos;
        std::tie(min_pos, max_pos) = std::minmax_element(data.begin(), data.end());

        double avg = std::accumulate(data.begin(), data.end(), 0.0) / data.size();

        return std::make_tuple(*min_pos, *max_pos, avg);
    }
}

template <typename T>
std::tuple<int, int, double> calc_stats(const T& data)
{
    const auto [min_pos, max_pos] = std::minmax_element(std::ranges::begin(data), std::ranges::end(data));

    double avg = std::accumulate(std::ranges::begin(data), std::ranges::begin(data), 0.0) / std::ranges::size(data);

    return std::tuple(*min_pos, *max_pos, avg);
}

TEST_CASE("Before C++17")
{
    int data[] = {4, 42, 665, 1, 123, 13};

    auto result = calc_stats(data);

    REQUIRE(std::get<0>(result) == 1);

    int min, max;
    double avg;

    std::tie(min, max, avg) = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}

TEST_CASE("Since C++17")
{
    std::vector<int> data = {4, 42, 665, 1, 123, 13};

    const auto [min, max, avg] = BeforeCpp17::calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}

auto get_coordinates() -> int (&)[2]
{
    static int coord[] = {1, 2};
    return coord;
}

std::array<int, 3> get_coord_3D()
{
    return {1, 2, 3};
}

struct ErrorCode
{
    int ec;
    const char* m;
};

ErrorCode open_file()
{
    return {13, "file not found"};
}

TEST_CASE("structured bindings")
{
    SECTION("native arrays")
    {
        auto [x, y] = get_coordinates();

        REQUIRE(x == 1);
        REQUIRE(y == 2);
    }

    SECTION("std::array")
    {
        auto&& [x, y, z] = get_coord_3D();

        REQUIRE(z == 3);
    }

    SECTION("plain structs")
    {
        const auto [error_code, error_message] = open_file();

        REQUIRE(error_code == 13);
    }
}

struct Timestamp
{
    int h, m, s;
};

TEST_CASE("structured bindings - how it works")
{
    alignas(128) const auto [hours, minutes, seconds] = Timestamp {10, 9, 0};

    SECTION("is interpreted")
    {
        alignas(128) const auto entity = Timestamp {10, 9, 0};
        auto& hours = entity.h;
        auto& minutes = entity.m;
        auto& seconds = entity.s;
    }
}

TEST_CASE("use cases")
{
    SECTION("iteration over map")
    {
        std::map<int, std::string> dict = {{1, "one"}, {2, "two"}, {3, "three"}};

        for (const auto& [key, value] : dict)
        {
            std::cout << key << " - " << value << "\n";
        }
    }

    SECTION("for + multiple declarations")
    {
        std::list lst{"one", "two", "three"};

        for(auto [index, it] = std::pair(0, std::ranges::begin(lst)); it != std::ranges::end(lst); ++index, ++it)
        {
            std::cout << index << " - " << *it << "\n";
        }
    }
}