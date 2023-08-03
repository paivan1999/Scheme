#include <scheme.h>
#include <stdexcept>


Scheme::Scheme(){}
std::string Scheme::Evaluate(const std::string& expression){
    std::stringstream ss{expression};
    Tokenizer tokenizer{&ss};
    auto obj = Read(&tokenizer);
    auto evaluated = EvaluateObj(scope_,obj);
    auto result = ToString(ToObjPtr(evaluated));
    return result;
}
ValuePtr Scheme::DeadMaker(DeadPrototype func) {
    return std::make_shared<DeadFunction>(func);
}

std::string Scheme::RecursiveListToString(ObjPtr obj) {
    if (!obj) {
        return "";
    }
    if (auto cell = As<Cell>(obj)) {
        return ToString(cell->GetFirst()) + " " + RecursiveListToString(cell->GetSecond());
    }
    else {
        return ". " + ToString(obj) + " ";
    }
}
std::string Scheme::ToString(ObjPtr obj){
    if (Is<Procedure>(obj)) {
        return "procedure";
    }
    if (auto numb = As<Number>(obj)){
        return std::to_string(numb->Get());
    }
    if (auto name = As<Symbol>(obj)){
        return name->Get();
    }
    if (Is<EmptyList>(obj)){
        return "()";
    }
    std::string result("(");
    result += RecursiveListToString(obj);
    result.pop_back();
    result += ")";
    return result;
}
Scope Scheme::CreateGlobal() {
    Scope scope = Scope();
    auto single = SingleMaker(IsBoolean);
    scope["boolean?"] = DeadMaker(SingleMaker(IsBoolean));
    scope["symbol?"] = DeadMaker(SingleMaker(IsSymbol));
    scope["number?"] = DeadMaker(SingleMaker(IsNumber));
    scope["pair?"] = DeadMaker(SingleMaker(IsPair));
    scope["null?"] = DeadMaker(SingleMaker(IsNull));
    scope["list?"] = DeadMaker(SingleMaker(IsList));
    scope["abs"] = DeadMaker(SingleMaker(Abs));
    scope["not"] = DeadMaker(SingleMaker(Not));
    scope["car"] = DeadMaker(SingleMaker(Car));
    scope["cdr"] = DeadMaker(SingleMaker(Cdr));
    scope["quote"] = DeadMaker(Quote);
    // double
    scope["cons"] = DeadMaker(DoubleMaker(Cons));
    scope["list-ref"] = DeadMaker(DoubleMaker(ListRef));
    scope["list-tail"] = DeadMaker(DoubleMaker(ListTail));
    // double-modify
    scope["define"] = DeadMaker(Define);
    scope["set!"] = DeadMaker(Setter);
    scope["set-cdr!"] = DeadMaker(DoubleMaker(SetCdr));
    scope["set-car!"] = DeadMaker(DoubleMaker(SetCar));
    // many
    scope["+"] = DeadMaker(Plus);
    scope["-"] = DeadMaker(Minus);
    scope["/"] = DeadMaker(Divide);
    scope["*"] = DeadMaker(Mult);
    scope[">"] = DeadMaker(More);
    scope["<"] = DeadMaker(Less);
    scope[">="] = DeadMaker(MoreEq);
    scope["<="] = DeadMaker(LessEq);
    scope["="] = DeadMaker(Eq);
    scope["and"] = DeadMaker(And);
    scope["or"] = DeadMaker(Or);
    scope["if"] = DeadMaker(If);
    scope["max"] = DeadMaker(Max);
    scope["min"] = DeadMaker(Min);
    scope["list"] = DeadMaker(List);
    // lambda
    scope["lambda"] = std::make_shared<Lambda>();
    return scope;
}



