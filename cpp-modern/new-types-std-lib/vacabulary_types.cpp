#include "catch.hpp"

#include <algorithm>
#include <any>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

using namespace std::literals;

template <typename TCollection>
void print(const TCollection& coll, std::string_view desc)
{
    std::cout << desc << ": [ ";
    for (const auto& item : coll)
        std::cout << item << " ";
    std::cout << "]\n";
}

namespace UndefinedBehavior
{
    void open_file(std::string_view file_name)
    {
        FILE* f_input = fopen(file_name.data(), "r"); // UB
    }
}

//////////////////////////////////////////
// string_view

TEST_CASE("string_view") 
{
    std::string_view sv1;

    REQUIRE(sv1.data() == nullptr);
    REQUIRE(sv1.size() == 0);

    SECTION("conversions from cstring & std::string")
    {
        std::string_view sv_txt_1 = "text cstring";

        std::string str = "text std::string";
        std::string_view sv_txt_2 = str;

        REQUIRE(sv_txt_2 == "text std::string"sv);

        std::vector<std::string_view> words = {"abc"sv, "def"sv, "ghi"sv};
        print(words, " ------------------------- words"); // passing cstring literal
        std::string name = " ------------------------- words";
        print(words, name);                                // passing string as lvalue
        print(words, " ------------------------- words"s); // passing rvalue
    }

    SECTION("tokens & string_view")
    {
        std::string text = "abc def ghi";

        std::string_view token1 {text.data(), 3};
        std::cout << token1 << "\n";

        std::string_view token2 {text.data() + 4, 3};
        std::cout << token2 << "\n";

        text = "----------------------------------- abc"; // invalidates all string_view objects

        // std::cout << token1 << "\n"; // UB
    }
}

std::string_view get_prefix(std::string_view text, size_t length)
{
    return {text.data(), length};
}

TEST_CASE("beware of string_view")
{
    SECTION("safe")
    {
        std::string_view name = "Mr. Johnson";

        std::string_view prefix = get_prefix(name, 3);
        std::cout << "prefix: " << prefix << "\n";
    }

    SECTION("unsafe - dangling pointer in string_view")
    {
        std::string_view prefix = get_prefix("Mr. Brownstone"s, 3);
        std::cout << "prefix: " << prefix << "\n";
    }

    SECTION("dangling sv")
    {
        std::string_view text = "text"s; // dangling pointer in text
    }
}

TEST_CASE("NOT a null terminated string")
{
    std::array buffer = {'a', 'r', 'r', 'a', 'y'};

    std::string_view text {buffer.data(), buffer.size()};

    std::cout << text << "\n";
}

TEST_CASE("string_view in constexpr")
{
	constexpr std::array ids = { "autodesk"sv, "infotraining"sv, "adobe"sv };
}

TEST_CASE("conversion from string_view -> string")
{
	std::vector<std::string> words;

	std::string_view text = "abc";

	words.push_back(std::string(text));
}

TEST_CASE("different orders")
{
	std::vector<std::string> names = { "Jan", "Alan", "Dominik", "Ewa", "Anna" };
	
	std::ranges::sort(names);

	print(names, "names ascending");

	std::vector<std::string_view> names_by_length(names.begin(), names.end());
	std::ranges::sort(names_by_length, [](const auto& a, const auto& b) { return a.size() < b.size(); });

	print(names_by_length, "names by length");

}