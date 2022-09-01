#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <numeric>

#include "catch.hpp"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////
TEST_CASE("accumulate as fold")
{
    std::vector vec = {1.1, 3.2, 4.3};

    double result = std::accumulate(vec.begin(), vec.end(), 0.0);

    REQUIRE(result == Approx(8.6));
}


template<typename C, typename... Telems>
auto matches(const C& c, Telems... elems) 
{
    return (0ULL + ... + std::count(std::begin(c), std::end(c), elems));
}

TEST_CASE("matches - returns how many items is stored in a container")
{
    // Tip: use std::count() algorithm

    vector<int> v{1, 2, 3, 4, 5};

    REQUIRE(matches(v, 2, 5) == 2);
    REQUIRE(matches(v, 100, 200) == 0);
    REQUIRE(matches("abccdef", 'x', 'y', 'z') == 0);
    REQUIRE(matches("abccdef", 'a', 'c', 'f') == 4);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

class Gadget
{
public:
    virtual std::string id() const { return "a"; }
    virtual ~Gadget() = default;
};

class SuperGadget : public Gadget
{
public:
    std::string id() const override
    {
        return "b";
    }
};

// TODO
template<typename... TArgs>
auto make_vector(TArgs&&... args)
{
    static_assert(sizeof...(args) > 0, "make_vector needs at least one param");

    using TValue = std::common_type_t<TArgs...>;
	
	std::vector<TValue> v;
    v.reserve(sizeof...(args));

	(..., v.push_back(std::forward<TArgs>(args)));
	return v;
}

TEST_CASE("make_vector - create vector from a list of arguments")
{
    // Tip: use std::common_type_t<Ts...> trait

    using namespace Catch::Matchers;

    SECTION("ints")
    {
        std::vector<int> v = make_vector(1, 2, 3);

        REQUIRE_THAT(v, Equals(vector{1, 2, 3}));
    }

    SECTION("unique_ptrs")
    {
        auto ptrs = make_vector(make_unique<int>(5), make_unique<int>(6));

        REQUIRE(ptrs.size() == 2);
    }

    SECTION("unique_ptrs with polymorphic hierarchy")
    {
        auto gadgets = make_vector(make_unique<Gadget>(), make_unique<SuperGadget>(), make_unique<Gadget>());

        static_assert(is_same_v<decltype(gadgets)::value_type, unique_ptr<Gadget>>);

        vector<string> ids;
        transform(begin(gadgets), end(gadgets), back_inserter(ids), [](auto& ptr) { return ptr->id(); });

        REQUIRE_THAT(ids, Equals(vector<string>{"a", "b", "a"}));
    }

    SECTION("make_vector with lvalue")
    {
        std::string str = "text";

        auto words = make_vector(str, str, str, "abc"s);

        REQUIRE(str == "text");

        REQUIRE(words == std::vector{"text"s, "text"s, "text"s, "abc"s});
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct range
{
    T low, high;
};

//deduction guide
template <typename T1, typename T2>
range(T1, T2) -> range<std::common_type_t<T1, T2>>;

template <typename T, typename... Args>
bool within(const range<T> &input, const Args &...args)
{
    // bool result = true;

    // (..., (result = result && (input.low <= static_cast<T1>(args))));
    // (..., (result = result && (static_cast<T2>(args) <= input.high)));

    // return result;

    const auto is_inside = [input](const auto& x) {
        return input.low <= x && x <= input.high;
    };

    return (... && is_inside(args));
}

TEST_CASE("within - checks if all values fit in range [low, high]")
{
    REQUIRE(within(range{10, 20.0}, 1, 15, 30) == false);  
    REQUIRE(within(range{10, 20}, 11, 12, 13) == true);
    REQUIRE(within(range{5.0, 5.5}, 5.1, 5.2, 5.3) == true);
}