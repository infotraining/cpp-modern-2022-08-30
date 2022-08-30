#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <span>
#include <utility>
#include <bitset>

#include "catch.hpp"

enum class DayOfWeek : uint8_t { mon = 1, tue, wed, thd, fri, sat, sun };

TEST_CASE("changes for enums")
{
    DayOfWeek today = DayOfWeek::tue;

    auto value = static_cast<std::underlying_type_t<DayOfWeek>>(today);

    REQUIRE(value == 2);

    SECTION("Since C++17")
    {
        DayOfWeek tomorrow{3};

        REQUIRE(tomorrow == DayOfWeek::wed);
    }
}

TEST_CASE("old school byte")
{
    char my_byte_1 = 32;
    char my_byte_2 = 33;
    auto result = my_byte_1 + my_byte_2;

    std::cout << my_byte_1 << "\n";
}

namespace Explain
{
    enum class byte : unsigned char {};

    constexpr byte operator &(byte a, byte b)
    {
        return a & b;
    }
}

TEST_CASE("std::byte")
{
    std::byte my_byte_1{32};
    std::byte my_byte_2{33};

    auto result = my_byte_1 & my_byte_2;    

    std::cout << std::to_integer<int>(result) << "\n";
}

void print(float const x, std::span<const std::byte> const bytes)
{
    std::cout << std::setprecision(6) << std::setw(8) << x << " = { "
              << std::hex << std::uppercase << std::setfill('0');
    
    for (auto const b : bytes) {
        std::cout << std::setw(2) << std::to_integer<int>(b) << ' ';
    }
    
    std::cout << std::dec << "}\n";
}

TEST_CASE("span of bytes")
{
    float data[1] { 3.141592f };

    auto const const_bytes = std::as_bytes(std::span{ data });

    print(data[0], const_bytes);


    auto const writable_bytes = std::as_writable_bytes(std::span{ data });
 
    // Change the sign bit that is the MSB (IEEE 754 Floating-Point Standard).
    writable_bytes[3] |= std::byte{ 0b1000'0000 };
 
    print(data[0], const_bytes);
}

TEST_CASE("bitset")
{
    std::bitset<8> bs1{"01010101"};

    std::cout << bs1.to_string() << " - " << bs1.to_ulong() << "\n";
}


/////////////////////////////////////////////////////////
// noexcept as a part of type

void foo()
{}

void bar() noexcept
{}

void call(auto f1, auto f2)
{
    f1();
    f2();
}

TEST_CASE("noexcept")
{
    static_assert(!std::is_same_v<decltype(foo), decltype(bar)>);

    void (*ptr_safe)() noexcept{};

    ptr_safe = bar;
    // ptr = foo; // ERROR

    void (*ptr_unsafe)();

    ptr_unsafe = foo;
    ptr_unsafe = bar;
    
    call(foo, bar);
}