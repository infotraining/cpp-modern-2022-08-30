#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

TEST_CASE("if with initializer")
{
    std::vector vec = {1, 2, 3, 42, 66, 34};

    if (auto pos = std::find(vec.begin(), vec.end(), 665); pos != vec.end())
    {
        std::cout << "Found: " << *pos << "\n";
    }
    else
    {
        assert(pos == vec.end());
        std::cout << "Item not found!\n";
    }
}

TEST_CASE("if with initializer + structured bindings")
{
    std::map<int, std::string> dict;

    if (auto [pos, is_inserted] = dict.insert(std::pair(42, "fourty two"));
        !is_inserted)
    {
        const auto& [key, value] = *pos;

        std::cout << key << " is already in a dictionary\n";
    }
}

TEST_CASE("if with mutex")
{
    std::queue<std::string> q_msg;
    std::mutex mtx_q_msg;

    SECTION("thread#1")
    {
        std::lock_guard lk {mtx_q_msg}; // CTAD
        q_msg.push("START");
    }

    SECTION("thread#2")
    {
        std::string msg;

        if (std::unique_lock lk {mtx_q_msg}; !q_msg.empty())
        {
            msg = q_msg.front();
            q_msg.pop();
        } // raii - automatic unlock
    }
}

///////////////////////////////////////////////////
// constexpr if

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral<T>::value, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point<T>::value, bool>
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

template <typename T>
bool is_power_of_2(T value)
{
    if constexpr (std::is_integral_v<T>)
        return value > 0 && (value & (value - 1)) == 0;
    else
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

TEST_CASE("constexpr if")
{
    REQUIRE(is_power_of_2(8));
    REQUIRE(is_power_of_2(64));
    REQUIRE(is_power_of_2(1024));
    REQUIRE(is_power_of_2(77) == false);

    REQUIRE(is_power_of_2(8.0));
}

namespace BeforeCpp17
{
    void print()
    {
        std::cout << "\n";
    }

    template <typename THead, typename... TTail>
    void print(const THead& head, const TTail&... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
}

template <typename THead, typename... TTail>
void print(const THead& head, const TTail&... tail)
{
    std::cout << head << " ";

    if constexpr (sizeof...(tail) > 0)
        print(tail...);
    else
        std::cout << "\n";
}

TEST_CASE("variadic print")
{
    print(1, 3.14, "text");
}