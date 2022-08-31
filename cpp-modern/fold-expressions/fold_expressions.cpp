#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

namespace VariadicTemplates
{
    template <typename... Ts>
    struct X
    {
        static constexpr size_t value = sizeof...(Ts);
    };

    template <size_t... Indexes>
    struct IndexSequence
    {
    };

    template <typename... Ts>
    struct Inheriter : Ts...
    {
    };
};

struct Car
{
    void drive() { }
};

struct Boat
{
    void swim() { }
};

TEST_CASE("variadic templates")
{
    VariadicTemplates::X<int, double, std::string, const std::vector<int>&> x1;
    VariadicTemplates::X<> x2;

    static_assert(VariadicTemplates::X<int, double>::value == 2);
    static_assert(VariadicTemplates::X<>::value == 0);

    VariadicTemplates::IndexSequence<0, 1, 2, 3, 4> is;

    VariadicTemplates::Inheriter<Car, Boat> amph;

    amph.drive();
}

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

namespace SinceCpp17
{
    namespace ConstexprIf
    {
        template <typename THead, typename... TTail>
        auto sum(const THead& head, const TTail&... tail)
        {
            if constexpr (sizeof...(tail) > 0)
                return head + sum(tail...);
            else
                return head;
        }
    }

    inline namespace FoldExpressions
    {
        template <typename... TArgs>
        auto sum(const TArgs&... args)
        {
            return (... + args); // left unary fold
        }

        template <typename... TArgs>
        void print(const TArgs&... args)
        {
            auto with_space = [is_first = true](const auto& item) mutable
            {
                if (!is_first)
                    std::cout << " ";
                is_first = false;
                return item;
            };

            (std::cout << ... << with_space(args)) << "\n"; // binary left fold
        }

        template <typename... TArgs>
        void print_lines(const TArgs&... args)
        {
            (..., (std::cout << args << "\n")); // left fold for comma operator
        }
    }
}

template <typename... Ts>
struct Head;

template <typename THead, typename... TTail>
struct Head<THead, TTail...>
{
    using type = THead;
};

TEST_CASE("Head")
{
    static_assert(std::is_same_v<Head<int, double>::type, int>);
}

template <typename F, typename... TArgs>
auto fill_vector(F f, TArgs&&... args)
{
    using TValue = std::common_type_t<decltype(f(args))...>;

    std::vector<TValue> vec;
    vec.reserve(sizeof...(args));

    (..., vec.push_back(f(std::forward<TArgs>(args))));

    return vec;
}

TEST_CASE("Before C++17")
{
    using namespace SinceCpp17;

    REQUIRE(sum(1, 2, 3, 4, 5) == 15);

    print(1, 3.14, "text");

    print_lines(1, 3.14, "text");

    auto square = [](int x) { return x * x;};

    auto squares = fill_vector(square, 1, 2, 3, 4, 5);

    REQUIRE(squares == std::vector{1, 4, 9, 16, 25});

    static_assert(std::is_same_v<std::common_type_t<int, long, short>, long>);
}