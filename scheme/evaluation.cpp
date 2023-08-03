//
// Created by PASHA on 23.02.2023.
//
#include <evaluation.h>
#include <iostream>

enum class ValueType { VALUE, DEAD, ALIVE, LAMBDA, CONST, UNDEF };
const std::unordered_set<ValueType> kScopeFunction{ValueType::LAMBDA, ValueType::DEAD};
const std::unordered_set<ValueType> kProcedure{ValueType::DEAD, ValueType::ALIVE};

Scope::Scope(Scope* previous) : previous_(previous) {
}
Scope::Scope() {
}
bool Scope::Has(std::shared_ptr<Symbol> symb) {
    return scope_loc_->contains(symb->Get());
}
Scope* Scope::Search(std::shared_ptr<Symbol> symb) {
    if (Has(symb)) {
        return this;
    } else {
        if (!previous_) {
            return nullptr;
        }
        return previous_->Search(symb);
    }
}
ValuePtr& Scope::operator[](std::shared_ptr<Symbol> symb) {
    return (*scope_loc_)[symb->Get()];
}
ValuePtr& Scope::operator[](std::string name) {
    return (*scope_loc_)[name];
}
ValuePtr Scope::operator[](std::shared_ptr<Symbol> symb) const {
    return (*scope_loc_)[symb->Get()];
}
ValuePtr Scope::operator[](std::string name) const {
    return (*scope_loc_)[name];
}
void Scope::Reset(std::shared_ptr<Symbol> symb, ValuePtr val) {
    if (auto value = AsV<ConstValue>(val)) {
        (*scope_loc_)[symb->Get()].swap(val);
    }
    if (auto value = AsV<UnDefValue>(val)) {
        (*scope_loc_)[symb->Get()].swap(val);
    }
}

Scope Scope::New() {
    return Scope(this);
}

Value::~Value() = default;
Value::Value() {
}
ValuePtr Value::operator()(Scope&,VecObjPtr) {
    return nullptr;
}
ValueType Value::GetType() {
    return ValueType::VALUE;
}

ValueType UnDefValue::GetType() {
    return ValueType::UNDEF;
}

ConstValue::ConstValue(ObjPtr value) {
    if (value->to_compute || Is<Procedure>(value) || Is<UnDef>(value)) {
        throw std::runtime_error("VALUE IS NOT CONST!!!!!!!!!!!!!!!!!!!!!!!!!");
    }
    value_ = value;
}
ObjPtr ConstValue::GetValue() {
    return value_;
}
ValueType ConstValue::GetType() {
    return ValueType::CONST;
}

AliveLambda::AliveLambda(Scope& scope, VecObjPtr args) : scope_(scope.New()) {
    if (args.Size() < 2) {
        throw SyntaxError("incorrect lambda");
    }
    auto first = args[0];
    if (IsCell(first)) {
        arg_names_ = VecObjPtr(first);
    } else {
        throw RuntimeError("incorrect argument names for lambda");
    }
    operators_ = args.Next();
}
void AliveLambda::RecursiveDefine(VecObjPtr arg_names, VecObjPtr arg) {
    if (!arg && !arg_names) {
        return;
    }
    if (arg && arg_names) {
        if (auto name = As<Symbol>(arg_names[0])) {
            ValuePtr value_ptr = EvaluateObj(scope_, arg[0]);
            scope_[name] = value_ptr;
            return RecursiveDefine(arg_names.Next(), arg.Next());
        } else {
            throw RuntimeError("all arguments names should be symbols");
        }
    }
    throw RuntimeError("incorrect count of arguments");
}
ValuePtr AliveLambda::RecursiveEvaluateOperators(VecObjPtr operators) {
    if (operators.Size() == 1) {
        return EvaluateObj(scope_, operators[0]);
    } else {
        EvaluateObj(scope_, operators[0]);
        return RecursiveEvaluateOperators(operators.Next());
    }
}
ValuePtr AliveLambda::operator()(Scope&, VecObjPtr arg) {
    RecursiveDefine(arg_names_, arg);
    return RecursiveEvaluateOperators(operators_);
}
ValueType AliveLambda::GetType() {
    return ValueType::ALIVE;
}

DeadFunction::DeadFunction(DeadPrototype func) : func_(func) {
}
ValuePtr DeadFunction::operator()(Scope& scope, VecObjPtr arg) {
    if (auto func_ptr = std::get_if<Single>(&func_)) {
        if (arg.Size() == 1) {
            return ToValuePtr((*func_ptr)(scope, arg[0]));
        }
        throw RuntimeError("should be definitely one arg");
    }
    if (auto func_ptr = std::get_if<Double>(&func_)) {
        if (arg.Size() == 2) {
            return ToValuePtr((*func_ptr)(scope, arg[0], arg[1]));
        }
        throw SyntaxError("should be definitely two args");
    }
    if (auto func_ptr = std::get_if<Many>(&func_)) {
        return ToValuePtr((*func_ptr)(arg, scope));
    }
    throw std::runtime_error("incorrect type of function");  // useless!
}
DeadPrototype DeadFunction::GetFunc() {
    return func_;
}
ValueType DeadFunction::GetType() {
    return ValueType::DEAD;
}

Lambda::Lambda() {
}
ValuePtr Lambda::operator()(Scope& scope, VecObjPtr vec) {
    return std::make_shared<AliveLambda>(scope, vec);
}
ValueType Lambda::GetType() {
    return ValueType::LAMBDA;
}

ValuePtr ToValuePtr(ObjPtr obj) {
    if (!obj) {
        return std::make_shared<ConstValue>(std::make_shared<EmptyList>());
    }
    if (Is<UnDef>(obj)) {
        return std::make_shared<UnDefValue>();
    }
    if (auto proc = As<Procedure>(obj)) {
        return proc->Get();
    }
    return std::make_shared<ConstValue>(obj);
}

ObjPtr ToObjPtr(ValuePtr val) {
    if (auto const_obj = AsV<ConstValue>(val)) {
        return const_obj->GetValue();
    }
    if (IsV<UnDef>(val)) {
        throw std::runtime_error("it can't be UnDef");
    }
    return std::make_shared<Procedure>(val);
}

ValuePtr EvaluateObj(Scope& scope, ObjPtr obj) {
    if (!obj) {
        throw RuntimeError("can't evaluate empty list!");
    }
    if (auto proc = As<Procedure>(obj)) {
        return proc->Get();
    }
    if (!obj->to_compute) {
        return ToValuePtr(obj);
    }
    if (auto symb = As<Symbol>(obj)) {
        if (auto found = scope.Search(symb)) {
            return (*found)[symb];
        }
        throw NameError("there isn't such name in scope");
    }
    auto list = VecObjPtr(obj);
    if (list.Size() == 0) {
        throw RuntimeError("can't calculate empty list");
    }
    auto func = EvaluateObj(scope, list[0]);
    if (!kProcedure.contains(func->GetType()) && !kScopeFunction.contains(func->GetType())) {
        throw RuntimeError("first element of list should be function or lambda");
    }
    auto new_list = list.Next();
    return (*func)(scope, new_list);
}
ObjPtr CheckConst(Scope& scope, ObjPtr obj) {
    if (auto val = AsV<ConstValue>(EvaluateObj(scope, obj))) {
        return val->GetValue();
    }
    throw std::runtime_error("it should be Const object");
}

ObjPtr CheckCommon(Scope& scope, ObjPtr obj) {
    auto val = EvaluateObj(scope, obj);
    if (auto val_const = AsV<ConstValue>(val)) {
        return val_const->GetValue();
    }
    if (kProcedure.contains(val->GetType())) {
        return std::make_shared<Procedure>(val);
    }
    return std::make_shared<EmptyList>();
}
