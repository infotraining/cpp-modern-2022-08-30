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

////////////////////
// TypeSize

template <typename T>
struct TypeSize
{
    static constexpr size_t value = sizeof(T);
};

// template variable
template <typename T>
constexpr size_t TypeSize_v = TypeSize<T>::value;

///////////////////
// Identity

template <typename T>
struct Identity
{
    using type = T;
};

template <typename T>
using Identity_t = typename Identity<T>::type;

TEST_CASE("concepts")
{
    static_assert(TypeSize<int>::value == 4);
    static_assert(TypeSize_v<int> == 4);

    static_assert(std::is_same_v<int, Identity<int>::type>);
    static_assert(std::is_same_v<int, Identity_t<int>>);
}

//////////////////////////////
// ElementT

template<typename C>
struct ElementT
{
    using type = typename C::value_type;
};

template<typename T, std::size_t N>
struct ElementT<T[N]>
{
    using type = T;
};

template<typename T>
struct ElementT<T[]>
{
    using type = T;
};

TEST_CASE("using ElementT")
{
    static_assert(std::is_same_v<ElementT<std::vector<int>>::type, int>);

    int tab[10];
    static_assert(std::is_same_v<ElementT<decltype(tab)>::type, int>);
}

//////////////////////////////////////////////////
// building blocks

TEST_CASE("integral constant")
{
    static_assert(std::integral_constant<int, 42>::value == 42);
    static_assert(std::integral_constant<bool, true>::value == std::bool_constant<true>::value);
    static_assert(std::true_type::value == true);
    static_assert(std::false_type::value == false);
}

///////////////////////////////////////////////////
// RemoveRef

template <typename T>
struct RemoveRef 
{
    using type = T;
};

template <typename T>
struct RemoveRef<T&>
{
    using type = T;
};

template <typename T>
struct RemoveRef<T&&>
{
    using type = T;
};

template <typename T>
using RemoveRef_t = typename RemoveRef<T>::type;

/////////////////////////////////////////////////
// IsIntegral

template <typename T>
struct IsIntegral : std::false_type
{};

template <>
struct IsIntegral<int> : std::true_type
{};

template <>
struct IsIntegral<long> : std::true_type {};

template <typename T>
constexpr bool IsIntegral_v = IsIntegral<T>::value;

///////////////////////////////////////
// IsPointer
template <typename T>
struct IsPointer : std::false_type{};
 
template <typename T>
struct IsPointer<T*> : std::true_type{};
 
template <typename T>
constexpr bool IsPointer_v = IsPointer<T>::value;

template <typename T>
void foo(T&& value)
{
    static_assert(!IsIntegral_v<T>);

    RemoveRef_t<T> backup{};

    //...

    backup = value;
}

///////////////////////////////////////////////////
// enable_if

template <bool Condition, typename T = void>
struct EnableIf
{
    using type = T;
};

template <typename T>
struct EnableIf<false, T>
{};

template <bool Condition, typename T>
using EnableIf_t = typename EnableIf<Condition, T>::type;


template <typename T>
auto sfinae(T obj) -> EnableIf_t<std::is_integral_v<T>, T>
{
    std::cout << "sfinae(integral)\n";

    return obj;
}

template <typename T>
auto sfinae(T obj) -> std::enable_if_t<std::is_floating_point_v<T>, T>
{
    std::cout << "sfinae(floating_points)\n";

    return obj;
}

TEST_CASE("sfinae")
{
    sfinae(42);
    sfinae(3.14);
    //sfinae(std::vector{1, 2, 3});
}

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral_v<T>, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point_v<T>, bool>
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

namespace SinceCpp17
{
    template <typename T>
    auto is_power_of_2(T value)
    {
        if constexpr (std::is_integral_v<T>)
        {
            return value > 0 && (value & (value - 1)) == 0;
        }
        else
        {
            int exponent;
            const T mantissa = std::frexp(value, &exponent);
            return mantissa == static_cast<T>(0.5);
        }
    }
}

/////////////////////////////////////
// void_t - concepts before C++20

namespace BeforeCpp20
{
    template <typename, typename = void>
    constexpr bool is_iterable_v {};

    template <typename T>
    constexpr bool is_iterable_v<
        T,
        std::void_t<
            decltype(std::begin(std::declval<T>())),
            decltype(std::end(std::declval<T>()))>> = true;

    template <typename T>
    auto print(const T& coll) -> std::enable_if_t<is_iterable_v<T>>
    {
        for(const auto& item : coll)
            std::cout << item << " ";
        std::cout << "\n";
    }
}

TEST_CASE("using void_t")
{
    static_assert(BeforeCpp20::is_iterable_v<std::vector<int>>);

    std::vector vec = {1, 2, 3};
    
    BeforeCpp20::print(vec);
}