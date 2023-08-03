#pragma once

#include <object.h>
#include <tokenizer.h>
std::shared_ptr<Object> MyRead(Tokenizer*, bool);
std::shared_ptr<Object> RecursiveReadList(Tokenizer* tokenizer);
std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);

std::shared_ptr<Object> Read(Tokenizer* tokenizer);
