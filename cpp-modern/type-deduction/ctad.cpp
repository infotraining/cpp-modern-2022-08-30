#include "catch.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using namespace std::literals;

template <typename T1, typename T2>
struct ValuePair
{
    T1 fst;
    T2 snd;

    ValuePair() = default;

    ValuePair(const T1& f, const T2& s)
        : fst {f}
        , snd {s}
    {
    }
};

// deduction guide
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

ValuePair(const char*, const char*)->ValuePair<std::string, std::string>;

TEST_CASE("CTAD")
{
    ValuePair<int, double> v1 {42, 3.14}; // C++98

    ValuePair v2 {42, 3.14}; // ValuePair<int, double>

    const int cx = 10;

    ValuePair v3 {cx, "text"s}; // ValuePair<int, std::string>

    ValuePair v4 {cx, "text"}; // ValuePair<int, const char*>

    // ValuePair v5; // ValuePair<int, double>

    ValuePair v6 {"text_a", "text_b"};
}

TEST_CASE("special case")
{
    std::vector vec = {1, 2, 3}; // vector<int>

    std::vector vec_alt {vec}; // vector<int> // SPECIAL CASE - CTAD

    std::vector vec_other {vec, vec}; // vector<vector<int>>
}

///////////////////////////////////////////
// CTAD + aggregates

template <typename T>
struct ValueWrapper
{
    T value;
};

// since C++20 compiler creates it implicitly
// template <typename T>
// ValueWrapper(T) -> ValueWrapper<T>;

TEST_CASE("CTAD & aggregates")
{
    ValueWrapper vw {10}; // ValueWrapper<int>
}

/////////////////////////////////////
// deduction guides in std lib

TEST_CASE("pair")
{
    auto p1 = std::make_pair(2, 3.14);

    auto p2 = std::pair(2, 3.14);

    std::pair p3(1, "text");
}

TEST_CASE("tuple")
{
    std::tuple tpl1 {1, 3.14, "text"s};

    auto p2 = std::pair(2, 3.14);
    std::tuple tpl2 = p2;
}

TEST_CASE("optional")
{
    std::optional<int> opt1 = 42;
    std::optional opt2 = 42; // std::optional<int>

    std::optional opt3 = opt2; // std::optional<int> - SPECIAL CASE - copy syntax
}

TEST_CASE("smart pointers")
{
    auto uptr = std::make_unique<int>(13);
    auto sptr = std::make_shared<int>(42);

    SECTION("conversions can use CTAD")
    {
        std::shared_ptr other_sptr = std::move(uptr);
        std::weak_ptr wptr = other_sptr;
    }
}

int foobar(int a, int b)
{
	return a + b;
}

TEST_CASE("function")
{
	std::function f = foobar;
}

TEST_CASE("containers")
{
	std::vector vec = {1, 2, 3};
	std::list lst = {"one"s, "two"s};

	std::vector vec_words(lst.begin(), lst.end());

	std::array arr1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
}