#pragma once

#include <parser.h>
#include <dead_functions.h>
#include <memory>
#include <sstream>
#include <object.h>
#include <evaluation.h>
#include <parser.h>
#include <tokenizer.h>

class Scheme {
public:
    Scheme();
    std::string Evaluate(const std::string& expression);
    Scope CreateGlobal();

private:
    ValuePtr DeadMaker(DeadPrototype func);

    std::function<ObjPtr(Scope&, ObjPtr)> SingleMaker(std::function<ObjPtr(ObjPtr)> func) {
        return [func](Scope& scope, ObjPtr arg) { return func(CheckCommon(scope, arg)); };
    }
    std::function<ObjPtr(Scope&, ObjPtr, ObjPtr)> DoubleMaker(
        std::function<ObjPtr(ObjPtr, ObjPtr)> func) {
        return [func](Scope& scope, ObjPtr arg1, ObjPtr arg2) {
            return func(CheckCommon(scope, arg1), CheckCommon(scope, arg2));
        };
    }

    std::string RecursiveListToString(ObjPtr obj);
    std::string ToString(ObjPtr obj);

    Scope scope_ = CreateGlobal();
};
