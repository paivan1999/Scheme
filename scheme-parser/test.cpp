#include <catch.hpp>

#include <sstream>
#include <random>

#include <parser.h>
#include <error.h>

std::shared_ptr<Object> ReadFull(const std::string& str) {
    std::stringstream ss{str};
    Tokenizer tokenizer{&ss};

    auto obj = MyRead(&tokenizer,true);
    REQUIRE(tokenizer.IsEnd());
    return obj;
}

void CheckNumber(const std::shared_ptr<Object>& obj, int value) {
    REQUIRE(Is<Number>(obj));
    REQUIRE(As<Number>(obj)->Get() == value);
}

void CheckSymbol(const std::shared_ptr<Object>& obj, const std::string& name) {
    REQUIRE(Is<Symbol>(obj));
    REQUIRE(As<Symbol>(obj)->Get() == name);
}

TEST_CASE("MyRead number") {
    auto node = ReadFull("5");
    CheckNumber(node, 5);

    node = ReadFull("-5");
    CheckNumber(node, -5);
}

std::string RandomSymbol(std::default_random_engine* rng) {
    std::uniform_int_distribution symbol('a', 'z');
    std::string s;
    for (auto i = 0; i < 5; ++i) {
        s.push_back(symbol(*rng));
    }
    return s;
}

TEST_CASE("MyRead symbol") {
    SECTION("Plus") {
        auto node = ReadFull("+");
        CheckSymbol(node, "+");
    }

    SECTION("Random symbol") {
        std::default_random_engine rng{42};
        for (auto i = 0; i < 10; ++i) {
            auto name = RandomSymbol(&rng);
            auto node = ReadFull(name);
            CheckSymbol(node, name);
        }
    }
}

TEST_CASE("Lists") {
    SECTION("Empty list") {
        auto null = ReadFull("()");
        REQUIRE(!null);
    }

    SECTION("Pair") {
        auto pair = ReadFull("(1 . 2)");
        REQUIRE(Is<Cell>(pair));
        auto cell = As<Cell>(pair);
        CheckNumber(cell->GetFirst(), 1);
        CheckNumber(cell->GetSecond(), 2);
    }

    SECTION("Simple list") {
        auto list = ReadFull("(1 2)");
        REQUIRE(Is<Cell>(list));
        auto cell = As<Cell>(list);
        CheckNumber(cell->GetFirst(), 1);

        cell = As<Cell>(cell->GetSecond());
        CheckNumber(cell->GetFirst(), 2);
        REQUIRE(!cell->GetSecond());
    }

    SECTION("List with operator") {
        auto list = ReadFull("(+ 1 2)");
        REQUIRE(Is<Cell>(list));
        auto cell = As<Cell>(list);

        CheckSymbol(cell->GetFirst(), "+");

        cell = As<Cell>(cell->GetSecond());
        CheckNumber(cell->GetFirst(), 1);

        cell = As<Cell>(cell->GetSecond());
        CheckNumber(cell->GetFirst(), 2);
        REQUIRE(!cell->GetSecond());
    }

    SECTION("List with funny end") {
        auto list = ReadFull("(1 2 . 3)");

        REQUIRE(Is<Cell>(list));
        auto cell = As<Cell>(list);
        CheckNumber(cell->GetFirst(), 1);

        cell = As<Cell>(cell->GetSecond());
        CheckNumber(cell->GetFirst(), 2);
        CheckNumber(cell->GetSecond(), 3);
    }

    SECTION("Complex lists") {
        ReadFull("(1 . ())");
        ReadFull("(1 2 . ())");
        ReadFull("(1 . (2 . ()))");
        ReadFull("(1 2 (3 4) (()))");
        ReadFull("(+ 1 2 (- 3 4))");
    }

    SECTION("Invalid lists") {
        REQUIRE_THROWS_AS(ReadFull("("), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1"), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1 ."), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("( ."), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1 . ()"), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1 . )"), SyntaxError);
        REQUIRE_THROWS_AS(ReadFull("(1 . 2 3)"), SyntaxError);
    }
}
