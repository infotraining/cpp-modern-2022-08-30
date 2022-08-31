#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
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

// template <typename T>
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

        auto a1 = x; // int
        deduce1(x);

        auto a2 = cx; // int
        deduce1(cx);

        auto a3 = ref_x; // int
        deduce1(ref_x);

        auto a4 = cref_x; // int
        deduce1(cref_x);

        auto a5 = tab; // int* : array decays to pointer
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

        auto& a1 = x; // int&
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

        auto&& a1 = x; // x is lvalue -> int&
        deduce3(x);    // deduce3<int&>(int& && arg -> int& arg)

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

    void push_back(const T& item) // push_back - makes copy
    {
        items_.push_back(item);
    }

    void push_back(T&& item) // always rvalue ref - no universal reference
    {
        items_.push_back(std::move(item)); // push_back - make move
    }

    template <typename... Ts>
    void emplace_back(Ts&&... args) // universal reference - type deduction context
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

TEST_CASE("auto + {}")
{
    auto lst = {1, 2, 3}; // std::initializer_list<int>

    SECTION("Before C++17")
    {
        int x1 = 10;
        int x2(10);
        int x3 {10};

        auto a1 = 10; // int
        auto a2(10);  // int
        auto a3 {10}; // std::initializer_list<int>
    }

    SECTION("Before C++17")
    {
        int x1 = 10;
        int x2(10);
        int x3 {10};

        auto a1 = 10; // int
        auto a2(10);  // int
        auto a3 {10}; // int
        // auto a4{10, 20}; // syntax error

        auto a4 = {1};       // std::initializer_list<int>
        auto a5 = {1, 2, 3}; // std::initializer_list<int>
    }
}

////////////////////////////////////////////
// decltype

namespace Cpp11
{
    template <typename T1, typename T2>
    auto multiply(T1 a, T2 b) -> decltype(a * b)
    {
        return a * b;
    }
}

template <typename T1, typename T2>
auto multiply(T1 a, T2 b)
{
    return a * b;
}

template <typename T>
auto bar()
{
    std::vector<decltype(std::declval<T>().foo())> foo_values;

    //...

    return foo_values;
}

TEST_CASE("decltype")
{
    std::map<int, std::string> dict = {{1, "one"}, {2, "two"}};

    auto dict_alt_1 = dict;
    REQUIRE(dict_alt_1.size() == 2);

    decltype(dict) dict_alt_2;
    REQUIRE(dict_alt_2.size() == 0);

    decltype(dict)::value_type value;

    auto result = Cpp11::multiply(1, 3.14);
}

template <typename TContainer>
decltype(auto) get_nth(TContainer& container, size_t n)
{
    return container[n];
}

TEST_CASE("auto vs. decltype auto")
{
    std::vector vec = {1, 2, 3};

    get_nth(vec, 2) = 5;

    REQUIRE(get_nth(vec, 2) == 5);

    SECTION("vector<bool>")
    {
        std::vector<bool> vec = {true, false, false};

        get_nth(vec, 2) = true;

        REQUIRE(get_nth(vec, 2) == true);
    }
}

template <typename T>
auto create_vec(T&& value)
{
    using TValue = std::decay_t<T>;

    std::vector<TValue> vec;

    // if constexpr (std::is_reference_v<T>)
    //     vec.push_back(value);  // makes copy
    // else
    //     vec.push_back(std::move(value));

    vec.push_back(std::forward<T>(value)); // perfect forwarding

    return vec;
}

namespace Explain
{
    template <typename T, typename... TArgs>
    std::unique_ptr<T> make_unique(TArgs&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
    }
}

TEST_CASE("forwarding")
{
    std::vector<int> vec_int = create_vec(1);

    std::string text = "abc";
    std::vector<std::string> vec_str_1 = create_vec(text);
    REQUIRE(vec_str_1.front() == text);

    std::vector<std::string> vec_str_2 = create_vec(std::string("rvalue"));
    REQUIRE(vec_str_2.front() == "rvalue");

    auto ptrs_1 = create_vec(std::make_unique<int>(13));

    auto ptr = Explain::make_unique<int>(42);
    auto ptrs_2 = create_vec(std::move(ptr));

    auto cstrings = create_vec("abc");

    std::vector<std::unique_ptr<int>> smart_ptrs = create_vec(std::make_unique<int>(13));
}

TEST_CASE("emplace")
{
    using namespace std::literals;

    std::vector<std::string> words;

    words.push_back("text"s); // move str to vec

    auto text = "text"s;
    words.push_back(text); // copy text to vec

    // words.push_back(std::string(10, 'a'));
    words.emplace_back(10, 'a');

    std::cout << words.back() << "\n";
}

//////////////////////////////////////////////
// initializer list

namespace InitializerList
{
    template <typename T>
    class Container
    {
        T* items_;
        size_t size_;

    public:
        Container(std::initializer_list<T> lst)
            : items_ {new T[lst.size()]}
            , size_ {lst.size()}
        {
            std::copy(lst.begin(), lst.end(), items_);
        }

        Container(size_t size, const T& value)
            : items_ {new T[size]}
            , size_ {size}
        {
            std::fill_n(items_, size_, value);
        }

        Container(const Container&) = delete;
        Container& operator=(const Container&) = delete;

        ~Container() noexcept
        {
            delete[] items_;
        }

        size_t size() const 
        {
            return size_;
        }
    };
}

TEST_CASE("initializer lists")
{
    using namespace std::literals;

    std::vector<std::string> words = {"abc"s, "def"s};

    InitializerList::Container<int> container = {1, 2, 3, 4};

    InitializerList::Container<int> container_alt{6, 10};
    REQUIRE(container_alt.size() == 2);

    InitializerList::Container<int> container_other(6, 10);
    REQUIRE(container_other.size() == 6);
}