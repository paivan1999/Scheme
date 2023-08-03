#include <catch.hpp>

#include <tokenizer.h>

#include <sstream>

template <class T, class... Args>
    requires(std::is_same_v<T, Args> || ...)
bool operator==(const std::variant<Args...>& v, const T& t) {
    auto* p = std::get_if<T>(&v);
    return p && (*p == t);
}

TEST_CASE("Tokenizer works on simple case") {
    std::stringstream ss{"4+)'."};
    Tokenizer tokenizer{&ss};

    REQUIRE(!tokenizer.IsEnd());

    // If next line fails to compile, check that operator == is defined for every token type.
    REQUIRE(tokenizer.GetToken() == ConstantToken{4});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == SymbolToken{"+"});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == BracketToken::CLOSE);

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == QuoteToken{});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == DotToken{});

    tokenizer.Next();
    REQUIRE(tokenizer.IsEnd());
}

TEST_CASE("Negative numbers") {
    std::stringstream ss{"-2 - 2"};
    Tokenizer tokenizer{&ss};

    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == ConstantToken{-2});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == SymbolToken{"-"});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == ConstantToken{2});
}

TEST_CASE("Symbol names") {
    std::stringstream ss{"foo bar zog-zog?"};
    Tokenizer tokenizer{&ss};

    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == SymbolToken{"foo"});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == SymbolToken{"bar"});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == SymbolToken{"zog-zog?"});
}

TEST_CASE("GetToken is not moving") {
    std::stringstream ss{"1234+4"};
    Tokenizer tokenizer{&ss};

    REQUIRE(tokenizer.GetToken() == ConstantToken{1234});
    REQUIRE(tokenizer.GetToken() == ConstantToken{1234});
}

TEST_CASE("Tokenizer is streaming") {
    std::stringstream ss;
    ss << "2 ";

    Tokenizer tokenizer{&ss};
    REQUIRE(tokenizer.GetToken() == ConstantToken{2});

    ss << "* ";
    tokenizer.Next();
    REQUIRE(tokenizer.GetToken() == SymbolToken{"*"});

    ss << "2";
    tokenizer.Next();
    REQUIRE(tokenizer.GetToken() == ConstantToken{2});
}

TEST_CASE("Spaces are handled correctly") {
    SECTION("Just spaces") {
        std::stringstream ss{"      "};
        Tokenizer tokenizer{&ss};

        REQUIRE(tokenizer.IsEnd());
    }

    SECTION("Spaces between tokens") {
        std::stringstream ss{"  4 +  "};
        Tokenizer tokenizer{&ss};

        REQUIRE(!tokenizer.IsEnd());
        REQUIRE(tokenizer.GetToken() == ConstantToken{4});

        tokenizer.Next();
        REQUIRE(!tokenizer.IsEnd());
        REQUIRE(tokenizer.GetToken() == SymbolToken{"+"});

        tokenizer.Next();
        REQUIRE(tokenizer.IsEnd());
    }
}

TEST_CASE("Literal strings are handled correctly") {
    SECTION("Only EOFs and newlines") {
        std::string input = R"EOF(
                                   )EOF";
        std::stringstream ss{input};
        Tokenizer tokenizer{&ss};

        REQUIRE(tokenizer.IsEnd());
    }

    SECTION("String with tokens") {
        std::string input = R"EOF(
                            4 +
                            )EOF";
        std::stringstream ss{input};
        Tokenizer tokenizer{&ss};

        REQUIRE(!tokenizer.IsEnd());
        REQUIRE(tokenizer.GetToken() == ConstantToken{4});

        tokenizer.Next();
        REQUIRE(!tokenizer.IsEnd());
        REQUIRE(tokenizer.GetToken() == SymbolToken{"+"});

        tokenizer.Next();
        REQUIRE(tokenizer.IsEnd());
    }
}

TEST_CASE("Empty string handled correctly") {
    std::stringstream ss;
    Tokenizer tokenizer{&ss};

    REQUIRE(tokenizer.IsEnd());
}
