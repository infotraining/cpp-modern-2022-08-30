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
#include <atomic>
#include <ranges>

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
    constexpr std::array ids = {"autodesk"sv, "infotraining"sv, "adobe"sv};
}

TEST_CASE("conversion from string_view -> string")
{
    std::vector<std::string> words;

    std::string_view text = "abc";

    words.push_back(std::string(text));
}

TEST_CASE("different orders")
{
    std::vector<std::string> names = {"Jan", "Alan", "Dominik", "Ewa", "Anna"};

    std::ranges::sort(names);

    print(names, "names ascending");

    std::vector<std::string_view> names_by_length(names.begin(), names.end());
    std::ranges::sort(names_by_length, [](const auto& a, const auto& b)
        { return a.size() < b.size(); });

    print(names_by_length, "names by length");
}

///////////////////////////////////////////////////
// any

TEST_CASE("std::any")
{
    std::any anything;

    REQUIRE(anything.has_value() == false);

    anything = 42;
    anything = 3.14;
    anything = "text"s;
    anything = std::vector {1, 2, 3};

    SECTION("any_cast returning copy")
    {
        auto vec = std::any_cast<std::vector<int>>(anything);
        REQUIRE(vec == std::vector {1, 2, 3});

        REQUIRE_THROWS_AS(std::any_cast<std::string>(anything), std::bad_any_cast);
    }

    SECTION("any_cast returning pointer")
    {
        anything = 3.14;

        double* ptr_pi = std::any_cast<double>(&anything);
        REQUIRE(ptr_pi != nullptr);
        *ptr_pi = 3.1415;
    }
}

////////////////////////////////////
// wide interfaces

class Observer
{
public:
    virtual void update(const std::any& sender, const std::string& msg) = 0;
    virtual ~Observer() = default;
};

class TempMonitor
{
    std::vector<Observer*> observes_;
public:
    void notify()
    {
        for(const auto& o : observes_)
            o->update(this, std::to_string(get_temp()));
    }
    double get_temp() const
    {
        return 23.88;
    }
};

class Logger : public Observer
{
public:
    void update(const std::any& sender, const std::string& msg) override
    {
        TempMonitor* const* monitor = std::any_cast<TempMonitor*>(&sender);
        
		if (monitor)
            (*monitor)->get_temp();
    }
};


/////////////////////////////////////////////////////////////////
// std::optional

TEST_CASE("optional")
{
	std::optional<int> opt_int;
	REQUIRE(opt_int.has_value() == false);

	opt_int = 42;
	REQUIRE(opt_int.has_value() == true);
	
	if (opt_int)
		std::cout << "opt_int has state\n"; 

	opt_int = std::nullopt; // the opt_int.reset();
	REQUIRE(opt_int.has_value() == false);

	SECTION("CTAD")
	{
		std::optional opt_int{42};

		std::optional other_opt_int{opt_int};
	}

	SECTION("construction in-place")
	{
		std::optional<std::atomic<int>> opt_counter{std::in_place, 0};
	}

	SECTION("using optional")
	{
		std::optional opt_int = 42;

		if (opt_int)
			REQUIRE(*opt_int == 42);

		*opt_int = 665;

		REQUIRE(opt_int.value() == 665);

		opt_int.reset();

		REQUIRE_THROWS_AS(opt_int.value(), std::bad_optional_access);
	}
}

std::optional<const char*> maybe_getenv(const char* n)
{
    if(const char* env_var = std::getenv(n))
        return env_var;
    else
        return std::nullopt;
}

TEST_CASE("value_or")
{
	std::cout << maybe_getenv("PATHS").value_or("(not found)") << "\n";
}

TEST_CASE("move semantics - beware")
{
	std::optional<std::string> opt_name = "Jan";
	REQUIRE(opt_name.has_value() == true);

	std::string target_name = std::move(*opt_name);
	REQUIRE(opt_name.has_value() == true);
}

TEST_CASE("optional - weird cases")
{
	std::optional<bool> flag{false};

	if (!flag) // yields false
	{		
	}

	if (flag == false) // yields true
	{		
	}

	std::optional<int*> ptr{nullptr};

	if (ptr) // yields true
	{
	}

	if (ptr == nullptr) // yields true
	{
	}
}

std::optional<int> to_int(std::string_view str)
{
	int value{};

    auto start = str.data();
    auto end = str.data() + str.size();

    if (const auto [pos_end, error_code] = std::from_chars(start, end, value);
        error_code != std::errc{} || pos_end != end)
    {
        return std::nullopt;
    }

    return value;
}

TEST_CASE("to_int")
{
	SECTION("happy path")
	{
		std::optional<int> number = to_int("42");
		REQUIRE(number.has_value());
		REQUIRE(*number == 42);
	}

	SECTION("sad path")
	{
		std::optional<int> number = to_int("42abc");
		REQUIRE(number.has_value() == false);
	}
}

////////////////////////////////////////////////////////////////
// std::variant

TEST_CASE("---")
{
	std::cout << "\n-----------------------------------------\n";
}

TEST_CASE("std::variant")
{
	std::variant<int, double, std::string, std::vector<int>> v1;

	REQUIRE(std::holds_alternative<int>(v1) == true);
	REQUIRE(std::get<int>(v1) == 0);

	v1 = 3.14;
	v1 = "text"s;
	v1 = std::vector{1, 2, 3};

	print(std::get<std::vector<int>>(v1), "v1");	

	REQUIRE_THROWS_AS(std::get<int>(v1), std::bad_variant_access);

	v1 = 665;
	int* ptr_int = std::get_if<int>(&v1);
	REQUIRE(ptr_int != nullptr);
}

struct Value
{
	int value;

	Value(int v) : value{v}
	{}
};

struct Wrapper
{
	int value;

	Wrapper(int v) : value{v}
	{}
};

TEST_CASE("monostate")
{
	std::variant<std::monostate, Value, Wrapper> v1;  

	REQUIRE(std::holds_alternative<std::monostate>(v1) == true);

	std::variant<std::monostate, Value> opt_value; // std::optional<Value>
}

TEST_CASE("emplace")
{
	std::variant<int, std::string, Value, int> v1;

	v1.emplace<std::string>(10, '*');
	v1.emplace<0>(42);
	v1.emplace<3>(665);

	REQUIRE(std::holds_alternative<std::string>(v1) == false);
	REQUIRE(v1.index() == 3);
}

// visitor
struct Printer
{
	std::string prefix;

	Printer(std::string prefix) : prefix{std::move(prefix)}
	{}

	void operator()(int i) const 
	{
		std::cout << "int: " << i << "\n";
	}

	void operator()(double d) const 
	{
		std::cout << "double: " << d << "\n";
	}

	void operator()(std::ranges::range auto const& v) const
	{
		print(v, "Container");
	}
};

template <typename... TClosures>
struct overloaded : TClosures...
{
	using TClosures::operator()...;
};

// C++17
// template <typename... TClosures>
// overloaded(TClosures...) -> overloaded<TClosures...>;


TEST_CASE("visiting variants")
{
	std::variant<int, double, std::string, std::vector<int>> v1 = std::vector{1, 2, 3};

	std::visit(Printer{" * "}, v1);

	std::string_view prefix = " + ";

	auto local_printer = overloaded {
		[prefix](int i) { std::cout << prefix <<  "int: " << i << "\n"; },
		[prefix](double d) { std::cout << prefix << "double: " << d << "\n"; },
		[prefix](std::ranges::range auto const&  rng) { 
			std::cout << prefix;
			print(rng, "Container"); 
		}
	};

	std::visit(local_printer, v1);
}

struct Lambda_74352376452654
{
	Lambda_74352376452654() = default;
	auto operator()(int a) const { return a * a; }
};

TEST_CASE("lambda")
{
	auto l = [](int a) { return a * a; };

	decltype(l) other_l; // since C++20

	SECTION("is interpreted as")
	{
		auto l = Lambda_74352376452654{};
	}
}