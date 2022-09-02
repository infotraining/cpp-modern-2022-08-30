#include "catch.hpp"

#include <algorithm>
#include <concepts>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

template <typename T>
concept IsPointer = requires(T ptr) { 
    *ptr; 
    ptr == nullptr;
    { ptr < ptr } -> std::convertible_to<bool>;
};

template <typename T>
concept IsLeanPointer = IsPointer<T> && requires(T x) {
    requires sizeof(x) == sizeof(intptr_t);
};

TEST_CASE("lean pointer")
{
    static_assert(IsLeanPointer<std::unique_ptr<int>>);
}

namespace Step1
{
    template <typename T>
        requires(!IsPointer<T>)
    T max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <typename T>
        requires IsPointer<T>
    auto max_value(T a, T b)
    {
        return max_value(*a, *b);
    }
}

namespace Step2
{
    template <typename T>
        requires(!IsPointer<T>)
    T max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <IsPointer T>
    auto max_value(T a, T b)
    {
        return max_value(*a, *b);
    }
}

template <typename T>
    requires(!IsPointer<T>)
T max_value(T a, T b)
{
    return a < b ? b : a;
}

auto max_value(IsPointer auto a, IsPointer auto b)
    requires std::three_way_comparable_with<decltype(*a), decltype(*b)>
{
    return max_value(*a, *b);
}

namespace Explain
{
    template <IsPointer T1, IsPointer T2>
    auto max_value(T1 a, T2 b)
    {
        return max_value(*a, *b);
    }
}

TEST_CASE("constraints")
{
    int x = 10;
    int y = 20;

    REQUIRE(max_value(x, y) == 20);

    REQUIRE(max_value(&x, &y) == 20);

    std::integral auto result = max_value(x, y);

    auto sp1 = std::make_shared<int>(13);
    auto sp2 = std::make_shared<int>(42);

    REQUIRE(max_value(sp1, sp2) == 42);
}

template <typename T>
concept PrintableRange = std::ranges::range<T> && requires {
    std::cout << std::declval<std::ranges::range_value_t<T>>();
};
 
void print(PrintableRange auto const& rng)
{
    for(const auto& item : rng)
        std::cout << item << " ";
    std::cout << "\n";
}

TEST_CASE("print with concepts")
{
    std::vector vec = {1, 2, 3};
    print(vec);

    std::map<int, std::string> dict = {{1, "one"}};
    //print(dict);
}

template <typename T>
struct Value
{
    T value;

    void print() const        
    {
        std::cout << value << "\n";
    }

    void print() const 
        requires std::ranges::range<T>
    {
        ::print(value);
    }
};

TEST_CASE("constraining member functions")
{
    Value v1{42};
    v1.print();

    Value v2{std::vector{1, 5, 6, 7, 8, 9}};
    v2.print();
}