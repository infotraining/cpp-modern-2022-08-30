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

template <std::ranges::range T> 
void print(T&& rng, std::string_view desc)
{
	std::cout << desc << ": [ ";
	for(const auto& item : rng)
		std::cout << item << " ";
	std::cout << "]\n";
}

constexpr int get_value(int n)
{
    return n * n;
}

constinit int global_value = get_value(42);

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
    std::cout << global_value << "\n";

    int v1 = runtime_func(8);

    int v2 = constexpr_func(8);

    static_assert(constexpr_func(8) == 64);

    v2 = consteval_func(13);

    auto square = [](int n) consteval { return n * n; };

    std::array squares { square(2), square(3), square(4) }; // std::array squares { 4, 9, 16) };
}

constexpr bool is_prime(int n)
{
    int limit;
    
    if (std::is_constant_evaluated())
        limit = n / 2;
    else
    {
        limit = std::sqrt(n);
        std::cout << "runtime check for "
                  << n << "\n";
    }

    for (int i = 2; i <= limit; ++i)
        if (n % i == 0)
            return false;
    return n > 1;
}

template <int N>
consteval std::array<int, N> prime_numbers()
{
    std::array<int, N> primes {};

    int idx = 0;
    for (int n = 1; idx < N; ++n)
    {
        if (is_prime(n))
        {
            primes[idx++] = n;
        }
    }
    return primes;
}

TEST_CASE("primes")
{
    int x = 13;

    REQUIRE(is_prime(x));

    static_assert(is_prime(13));

    constexpr std::array const_primes = prime_numbers<100>();

    std::array mutable_primes = prime_numbers<200>();
    mutable_primes[0] = -13;

    print(mutable_primes, "mp");
}

template <std::ranges::input_range Rng>
constexpr auto median(const Rng& rng)
{
    if (std::ranges::empty(rng))
        return 0.0;

    std::vector data(std::ranges::begin(rng), std::ranges::end(rng));

    std::ranges::sort(data);
    
    size_t size = std::ranges::size(data);
    size_t middle = size / 2;
    
    if (size % 2 == 0)
    {        
        return (data[middle - 1] + data[middle]) / 2.0;
    }

    return static_cast<double>(data[middle]);
}

TEST_CASE("median")
{
    constexpr std::array data{3, 2, 1};
	static_assert(median(data) == 2);
}