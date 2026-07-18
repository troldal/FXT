//
// Created by kenne on 18/07/2026.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <algorithm>
#include <string>
#include <vector>

using namespace std::string_literals;

namespace
{
    struct Config
    {
        int         retries {};
        std::string host;

        bool operator==(const Config&) const = default;
    };

    struct Timeout
    {
        double seconds {};
    };

    // A key used purely as identity, with an ASSOCIATED value stored under
    // it (the pattern domain wrappers use to keep per-entry metadata).
    struct SolverKey
    {
    };

    struct SolverEntry
    {
        std::string name;
        int         iterations {};
    };
}

TEST_CASE("fxt::type_map stores at most one value per key type", "[type_map]")
{
    fxt::type_map map;
    CHECK(map.empty());
    CHECK(map.size() == 0);

    SECTION("homogeneous set/get round-trips the value")
    {
        map.set<Config>(Config { 3, "example.org" });
        REQUIRE(map.contains<Config>());
        REQUIRE(map.size() == 1);

        const auto* config = map.get<Config>();
        REQUIRE(config != nullptr);
        CHECK(*config == Config { 3, "example.org" });
    }

    SECTION("setting the same key again replaces, never duplicates")
    {
        map.set<Config>(Config { 1, "first" });
        map.set<Config>(Config { 2, "second" });
        REQUIRE(map.size() == 1);
        CHECK(map.get<Config>()->host == "second");
    }

    SECTION("absence is a nullptr, not an error")
    {
        CHECK(map.get<Config>() == nullptr);
        CHECK_FALSE(map.contains<Timeout>());
    }

    SECTION("erase removes exactly the keyed entry")
    {
        map.set<Config>(Config { 1, "a" }).set<Timeout>(Timeout { 2.5 });
        REQUIRE(map.size() == 2);
        CHECK(map.erase<Config>());
        CHECK_FALSE(map.erase<Config>());    // already gone
        CHECK(map.size() == 1);
        CHECK(map.contains<Timeout>());
    }

    SECTION("clear empties the map")
    {
        map.set<Config>(Config {}).set<Timeout>(Timeout {});
        map.clear();
        CHECK(map.empty());
    }
}

TEST_CASE("fxt::type_map supports associated values under an identity key", "[type_map]")
{
    fxt::type_map map;
    map.set<SolverKey>(SolverEntry { "newton", 12 });

    // Retrieval names the stored type explicitly:
    const auto* entry = map.get<SolverKey, SolverEntry>();
    REQUIRE(entry != nullptr);
    CHECK(entry->name == "newton");
    CHECK(entry->iterations == 12);

    // Asking for the wrong stored type is a type mismatch, answered with
    // nullptr — never a throw, never a reinterpretation:
    CHECK(map.get<SolverKey>() == nullptr);             // SolverKey itself is not stored
    CHECK(map.get<SolverKey, Config>() == nullptr);     // nor is a Config
    CHECK(map.contains<SolverKey>());                   // but the key IS present
}

TEST_CASE("fxt::type_map::for_each visits exactly the entries of one stored type", "[type_map]")
{
    // Two keys sharing a uniform entry type (the domain-wrapper pattern),
    // plus one foreign entry that must be skipped.
    struct KeyA
    {
    };
    struct KeyB
    {
    };

    fxt::type_map map;
    map.set<KeyA>(SolverEntry { "a", 1 }).set<KeyB>(SolverEntry { "b", 2 }).set<Config>(Config { 9, "x" });

    std::vector<std::string> names;
    map.for_each<SolverEntry>([&](const SolverEntry& e) { names.push_back(e.name); });
    std::ranges::sort(names);
    CHECK(names == std::vector { "a"s, "b"s });

    int configs = 0;
    map.for_each<Config>([&](const Config&) { ++configs; });
    CHECK(configs == 1);
}

TEST_CASE("fxt::type_map copies are independent values", "[type_map]")
{
    fxt::type_map original;
    original.set<Config>(Config { 1, "original" });

    fxt::type_map copy = original;
    copy.set<Config>(Config { 2, "copy" });

    CHECK(original.get<Config>()->host == "original");
    CHECK(copy.get<Config>()->host == "copy");
}
