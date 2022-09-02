#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_set>

template <typename T>
void hash_combine(size_t& seed, const T& value)
{
    seed ^= std::hash<T> {}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename... Args>
size_t combined_hash(const Args&... args)
{
    size_t seed {};
    (..., hash_combine(seed, args));
    return seed;
}

template <typename... Ts>
auto calc_hash(const std::tuple<Ts...>& tpl)
{
    auto hashing = [](const auto&... args)
    {
        return combined_hash(args...);
    };

    return std::apply(hashing, tpl);
}

template <typename T>
struct TieHashable
{
    size_t hash() const
    {
        const T& self = static_cast<const T&>(*this);
        return calc_hash(self.tied());
    }

	bool operator==(const TieHashable&) const = default;
};

template <typename T>
concept Hashable = requires(T obj)
{
    { obj.hash() } -> std::same_as<size_t>;
};

template <Hashable TWithHash>
struct std::hash<TWithHash>
{
    size_t operator()(const TWithHash& obj) const
    {
        return obj.hash();
    }

};

struct Person
{
    int id;
    std::string name;

    Person(int id, std::string name)
        : id {id}
        , name {std::move(name)}
    {
    }

    auto tied() const
    {
        return std::tie(id, name);
    }

	bool operator==(const Person& other) const = default;

    size_t hash() const
    {
        return calc_hash(tied());
    }
};

namespace CRTP
{
    struct Person : TieHashable<Person>
    {
        int id;
        std::string name;

        Person(int id, std::string name)
            : id {id}
            , name {std::move(name)}
        {
        }

        auto tied() const
        {
            return std::tie(id, name);
        }

        bool operator==(const Person& other) const = default;
    };
}

TEST_CASE("hashing custom classes")
{
    SECTION("hash implemented in class")
    {
        Person p1 {42, "Kowalski"};
        REQUIRE(p1.hash() == 1067095340826668709ULL);
    }

    SECTION("hash injected using CRTP")
    {
        CRTP::Person p2 {42, "Kowalski"};
        REQUIRE(p2.hash() == 1067095340826668709ULL);
    }

    SECTION("unordered set")
    {
        SECTION("hash implemented in class")
        {
            std::unordered_set<Person> team;
            team.insert(Person {42, "Kowalski"});

            REQUIRE(team.count(Person {42, "Kowalski"}) == 1);
        }

        SECTION("hash injected using CRTP")
        {
            std::unordered_set<CRTP::Person> team;
            team.insert(CRTP::Person {42, "Kowalski"});

            REQUIRE(team.count(CRTP::Person {42, "Kowalski"}) == 1);
        }
    }
}