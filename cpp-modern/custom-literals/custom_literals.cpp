#include "catch.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using namespace std::literals;

namespace Unsafe
{
    double calculate_speed(double distance, double time)
    {
        return distance / time;
    }
}

TEST_CASE("unsafe code")
{
    double speed = Unsafe::calculate_speed(100.0, 10.0);
}