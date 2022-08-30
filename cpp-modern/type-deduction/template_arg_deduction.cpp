#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#define MSVC

#ifdef GCC_CLANG
#define PRINT_SIGNATURE __PRETTY_FUNCTION__
#else
#define PRINT_SIGNATURE __FUNCSIG__
#endif

using namespace std;

void foo(int)
{
}

//template <typename T>
void deduce1(auto arg)
{
    puts(PRINT_SIGNATURE);
}

template <typename T>
void deduce2(T& arg)
{
    puts(PRINT_SIGNATURE);
}

template <typename T>
void deduce3(T&& arg)
{
    puts(PRINT_SIGNATURE);
}

TEST_CASE("Type deduction rules")
{
    SECTION("Case1")
    {
        int x = 10;
        const int cx = 10;
        int& ref_x = x;
        const int& cref_x = x;
        int tab[10];

        auto a1 = x;      // int
        deduce1(x);

        auto a2 = cx;     // int
        deduce1(cx);

        auto a3 = ref_x;  // int
        deduce1(ref_x);

        auto a4 = cref_x; // int
        deduce1(cref_x); 

        auto a5 = tab;    // int* : array decays to pointer
        deduce1(tab);

        auto a6 = foo; // void (*a6)(int) : function decays to pointer
        deduce1(foo);
    }

    cout << "\n";

    SECTION("Case2")
    {
        int x = 10;
        const int cx = 10;
        int& ref_x = x;
        const int& cref_x = x;
        int tab[10];

        auto& a1 = x;  // int&
        deduce2(x);

        auto& a2 = cx; // const int&
        deduce2(cx);

        auto& a3 = ref_x; // int&
        deduce2(ref_x);

        auto& a4 = cref_x; // const int&
        deduce2(cref_x);

        auto& a5 = tab; // int (&)[10] : does not decay
        deduce2(tab);

        auto& a6 = foo; // int (&)() :  does not decay
        deduce2(foo);        
    }

    cout << "\n";

    SECTION("Case3 - type deduction")
    {
        int x = 10;
        const int cx = 10;
        int& ref_x = x;
        const int& cref_x = x;
        int tab[10];

        auto&& a1 = x;  // x is lvalue -> int&
        deduce3(x);     // deduce3<int&>(int& && arg -> int& arg)

        auto&& a2 = 42; // 42 is rvalue -> int&&
        deduce3(42);
    }
}

template <typename T>
class Container
{
    std::vector<T> items_;
public:
    Container() = default;
    
    void push_back(const T& item)
    {
        items_.push_back(item);
    }

    void push_back(T&& item) // always rvalue ref - no universal reference
    {
        items_.push_back(std::move(item));
    }

    template <typename... Ts>
    void emplace_back(Ts&&... args)
    {
        items_.emplace_back(std::forward<Ts>(args)...);
    }
};

TEST_CASE("using container")
{
    using namespace std::literals;

    Container<std::string> words;

    auto w = "text"s;

    words.push_back(w);
    words.push_back("hello"s);
    words.emplace_back(10, 'a');
}