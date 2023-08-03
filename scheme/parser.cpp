#include <parser.h>
#include <object.h>
#include <tokenizer.h>

std::shared_ptr<Object> RecursiveReadList(Tokenizer* tokenizer){
    auto first_token = tokenizer->GetToken();
    auto tmp1 = std::get_if<BracketToken>(&first_token);
    if (tmp1 && *tmp1 == BracketToken::CLOSE) {
        tokenizer->Next();
        return nullptr;
    }
    if (std::get_if<DotToken>(&first_token)) {
        tokenizer->Next();
        auto last = MyRead(tokenizer, false);
        auto close_token = tokenizer->GetToken(true);
        auto tmp3 = std::get_if<BracketToken>(&close_token);
        if (!(tmp3 && *tmp3 == BracketToken::CLOSE)) {
            throw SyntaxError("wrong format of Incorrect list");
        }
        return last;
    }
    else {
        auto first_element = MyRead(tokenizer,false);
        return std::make_shared<Cell>(first_element, RecursiveReadList(tokenizer));
    }
}
std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    auto first_token = tokenizer->GetToken();
    if (std::get_if<DotToken>(&first_token)) {
        throw SyntaxError("wrong dot");
    }
    return RecursiveReadList(tokenizer);
}

std::shared_ptr<Object> MyRead(Tokenizer* tokenizer, bool glob = true) {
    Token token = tokenizer->GetToken(true);
    if (std::get_if<DotToken>(&token)) {
        throw SyntaxError{"wrong dot"};
    }
    if (auto real_token = std::get_if<ConstantToken>(&token)) {
        return std::make_shared<Number>(real_token->value);
    }
    if (auto real_token = std::get_if<SymbolToken>(&token)) {
        return std::make_shared<Symbol>(real_token->name);
    }
    if (std::get_if<QuoteToken>(&token)) {
        auto tmp = MyRead(tokenizer, false);
        if (tmp) {
            tmp->to_compute = false;
        }
        if (glob && !tokenizer->IsEnd()) {
            throw SyntaxError("it should be the end, but it isn't");
        }
        if (tmp) {
            return tmp;
        }
        return std::make_shared<EmptyList>();
    } else {
        BracketToken* real_bracket_token = std::get_if<BracketToken>(&token);
        if (*real_bracket_token == BracketToken::OPEN) {
            auto result = ReadList(tokenizer);
            if (glob && !tokenizer->IsEnd()) {
                throw SyntaxError("it should be the end, but it isn't");
            }
            return result;

        } else {
            throw SyntaxError{"wrong close bracket"};
        }
    }
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    return MyRead(tokenizer);
}


