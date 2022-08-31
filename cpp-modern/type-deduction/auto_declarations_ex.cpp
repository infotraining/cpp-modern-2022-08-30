#include "catch.hpp"

#include <iostream>
#include <map>
#include <type_traits>
#include <vector>

using namespace std;

using TODO = void;

TEST_CASE("exercise")
{
    int x = 10;
    const int cx = 1;
    int tab[10] = {};
    vector<int> vec = {1, 2, 3};
    const vector<int> cvec = {1, 2, 3};

    auto a1 = x;
    static_assert(is_same<TODO, decltype(a1)>::value, "Error");

    auto a2 = &x;
    static_assert(is_same<TODO, decltype(a2)>::value, "Error");

    const auto a3 = &x;
    static_assert(is_same<TODO, decltype(a3)>::value, "Error");

    auto* a4 = &x;
    static_assert(is_same<TODO, decltype(a4)>::value, "Error");

    auto a5 = &cx;
    static_assert(is_same<TODO, decltype(a5)>::value, "Error");

    auto* a6 = &cx;
    static_assert(is_same<TODO, decltype(a6)>::value, "Error");

    int& rx = x;
    const int& crx = x;

    const auto& a7 = rx;
    static_assert(is_same<TODO, decltype(a7)>::value, "Error");

    auto a8 = crx;
    static_assert(is_same<TODO, decltype(a8)>::value, "Error");

    auto& a9 = crx;
    static_assert(is_same<TODO, decltype(a9)>::value, "Error");

    auto a10 = tab;
    static_assert(is_same<TODO, decltype(a10)>::value, "Error");

    auto& a11 = tab;
    static_assert(is_same<TODO, decltype(a11)>::value, "Error");

    auto a12 = vec.begin();
    static_assert(is_same<TODO, decltype(a12)>::value, "Error");

    auto a13 = vec.cbegin();
    static_assert(is_same<TODO, decltype(a13)>::value, "Error");

    auto a14 = vec.back();
    static_assert(is_same<TODO, decltype(a14)>::value, "Error");

    auto& a15 = cvec.back();
    static_assert(is_same<TODO, decltype(a15)>::value, "Error");

    const auto a16 = vec[0];
    static_assert(is_same<TODO, decltype(a16)>::value, "Error");

    auto&& a17 = 7.5;
    static_assert(is_same<TODO, decltype(a17)>::value, "Error");

    auto&& a18 = crx;
    static_assert(is_same<TODO, decltype(a18)>::value, "Error");

    auto&& a19 = vec.front();
    static_assert(is_same<TODO, decltype(a19)>::value, "Error");

    auto&& a20 = vec.begin();
    static_assert(is_same<TODO, decltype(a20)>::value, "Error");

    auto a21 = {1, 2, 3};
    static_assert(is_same<TODO, decltype(a21)>::value, "Error");

    auto a22{1};
    static_assert(is_same<TODO, decltype(a22)>::value, "Error");

    auto a23 = {"text"s};
    static_assert(is_same<TODO, decltype(a23)>::value, "Error");
}