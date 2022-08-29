#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <array>
#include <cmath>

#include "catch.hpp"

constinit int global_value = 10;

int runtime_func(int x)
{
    return x * x;
}
 
constexpr int constexpr_func(int x) 
{
    return x * x;
}
 
consteval int consteval_func(int x)
{
    return x * x;
}

TEST_CASE("runtime vs. constexpr vs. consteval")
{
}