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

TEST_CASE("Before C++17")
{
    std::vector<int> data = {4, 42, 665, 1, 123, 13};

    auto result = BeforeCpp17::calc_stats(data);

    REQUIRE(std::get<0>(result) == 1);

    int min, max;
    double avg;

    std::tie(min, max, avg) = BeforeCpp17::calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}
