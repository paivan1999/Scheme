#pragma once

#include <variant>
#include <istream>
#include <unordered_set>
#include <error.h>


struct SymbolToken {
    std::string name;
    SymbolToken(std::string name) : name(name) {
    }
    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    };
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    };
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    };
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;
    ConstantToken(int value) : value(value) {
    }
    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    };
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

// Интерфейс позволяющий читать токены по одному из потока.
class Tokenizer {
public:
    Tokenizer(std::basic_istream<char>* in) : in_(in) {
        std::string digits = "0123456789";
        std::string symb_firsts = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ<=>*#";
        std::string symb_after_firsts = "?!-";
        std::string specials = "().'+-/";
        digit_ = Set(digits);
        symb_first_ = Set(symb_firsts);
        symb_after_first_ = Set(digits + symb_firsts + symb_after_firsts);
        global_ = Set(digits + symb_firsts + symb_after_firsts + specials);
    }

    bool IsEnd() {
        Force();
        return in_->eof();
    };

    void Next() {
        if (IsEnd()) {
            throw SyntaxError("can't take next cause of EOF");
        }
        GetToken(true);
    };

    Token GetToken(bool to_next = false) {
        if (IsEnd()) {
            throw SyntaxError("can't take next cause of EOF");
        }
        Token result{ConstantToken(0)};
        char ch = in_->peek();
        if (Set("()\'.+-/").contains(ch)){
            if (Set("()\'./").contains(ch)) {
                if (ch == '(') {
                    result = BracketToken::OPEN;
                }
                if (ch == ')') {
                    result = BracketToken::CLOSE;
                }
                if (ch == '\'') {
                    result = QuoteToken();
                }
                if (ch == '.') {
                    result = DotToken();
                }
                if (ch == '/') {
                    result = SymbolToken("/");
                }
                if (to_next) {
                    in_->get();
                }
            }
            else {
                size_t current = in_->tellg();
                char sign = in_->get();
                char next = in_->peek();
                if (digit_.contains(next)) {
                    int n = GetInt(to_next);
                    if (sign == '-') {
                        n = -n;
                    }
                    result = ConstantToken(n);
                } else {
                    result = SymbolToken(std::string{sign});
                }
                if (!to_next) {
                    in_->seekg(current);
                }
            }
            return result;
        }
        else {
            if (digit_.contains(ch)) {
                return ConstantToken(GetInt(to_next));
            } else {
                return SymbolToken(GetStr(to_next));
            }
        }
    };

private:
    std::unordered_set<char> Set(std::string s){
        std::unordered_set<char> result;
        for (auto el : s) {
            result.insert(el);
        }
        return result;
    }
    void Force() {

        while (!in_->eof() && !global_.contains(in_->peek())) {
            in_->get();
        }
    }
    int GetInt(bool to_next = false) {
        std::string s{};
        size_t current = in_->tellg();
        while (!in_->eof()) {
            char ch = in_->peek();
            if (digit_.contains(ch)) {
                s.push_back(ch);
                in_->get();
            } else {
                break;
            }
        }
        if (!to_next) {
            in_->seekg(current);
        }
        return std::stoi(s);
    }
    std::string GetStr(bool to_next = false) {
        size_t current = in_->tellg();
        std::string s{static_cast<char>(in_->get())};
        while (!in_->eof()) {
            char ch = in_->peek();
            if (symb_after_first_.contains(ch)) {
                s.push_back(ch);
                in_->get();
            } else {
                break;
            }
        }
        if (!to_next) {
            in_->seekg(current);
        }
        return s;
    }
    std::unordered_set<char> digit_{};
    std::unordered_set<char> symb_first_{};
    std::unordered_set<char> symb_after_first_{};
    std::unordered_set<char> global_{};
    std::basic_istream<char>* in_;
};
