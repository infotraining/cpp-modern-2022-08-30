#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <ranges>
#include <string_view>

#include "catch.hpp"

template <std::ranges::range T> 
void print(T&& rng, std::string_view desc)
{
	std::cout << desc << ": [ ";
	for(const auto& item : rng)
		std::cout << item << " ";
	std::cout << "]\n";
}

template <std::ranges::view TView> 
void print(TView rng, std::string_view desc)
{
	std::cout << desc << ": [ ";
	for(const auto& item : rng)
		std::cout << item << " ";
	std::cout << "]\n";
}

template <auto Value_>
struct EndValue
{
    bool operator==(auto pos) const
    {
        return *pos == Value_;
    }
};

TEST_CASE("ranges")
{
	std::vector vec = {1, 4, 6, 34, 567, 87, 23, 343, 665, 42, 12, 7, 5 };

	SECTION("algorithms")
	{
		std::ranges::sort(vec);

		print(vec, "vec");
	}

	SECTION("projections")
	{
		std::vector<std::string> words{"c", "c++", "c++20", "template"};

		std::ranges::sort(words, [](const auto& a, const auto& b) { return a.size() < b.size() ;});
		print(words, "words");

		std::ranges::sort(words, std::greater{}, [](const auto& w) { return w.size(); });
		print(words, "words");
	}

	SECTION("sentinels")
	{
		std::ranges::sort(vec.begin(), EndValue<665>(), std::greater{});
		print(vec, "vec sorted with sentinel");

		auto slice = std::ranges::subrange(vec.begin(), EndValue<665>());
		
		print(slice, "slice");
	}

	SECTION("views")
	{
		auto all_items = std::views::all(vec);
		print(all_items, "all_items");


		auto head = vec | std::views::take(5);
		print(head, "head");

		auto tail = vec | std::views::drop(5);
		print(tail, "tail");

		auto evens = vec
			| std::views::filter([](int n) { return n % 2 == 0;})
			| std::views::transform([](int n) { return n * n;})
			| std::views::take(5)
			| std::views::reverse;

		print(evens, "evens");

		std::vector evens_vec(evens.begin(), evens.end());

		std::map<int, std::string> dict = { {1, "one"}, {2, "two"} };

		print(dict | std::views::keys, "keys");
		print(dict | std::views::values, "values");
	}
}