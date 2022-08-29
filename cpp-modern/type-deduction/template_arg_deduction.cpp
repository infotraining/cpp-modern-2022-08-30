#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#define MSVC

#ifdef GCC_CLANG
#define PRINT_SIGNATURE __PRETTY_FUNCTION__
#else
#define PRINT_SIGNATURE __FUNCSIG__
#endif

using namespace std;

void foo(int)
{
}

template <typename T>
void deduce1(T arg)
{
    puts(PRINT_SIGNATURE);
}

template <typename T>
void deduce2(T& arg)
{
    puts(PRINT_SIGNATURE);
}

template <typename T>
void deduce3(T&& arg)
{
    puts(PRINT_SIGNATURE);
}

TEST_CASE("Type deduction rules")
{
    SECTION("Case1")
    {
        int x = 10;
        const int cx = 10;
        int& ref_x = x;
        const int& cref_x = x;
        int tab[10];
    }

    cout << "\n";

    SECTION("Case2")
    {
        int x = 10;
        const int cx = 10;
        int& ref_x = x;
        const int& cref_x = x;
        int tab[10];

        
    }

    cout << "\n";

    SECTION("Case3 - type deduction")
    {
        int x = 10;
        const int cx = 10;
        int& ref_x = x;
        const int& cref_x = x;
        int tab[10];
    }
}