#pragma once

#ifndef SHAD_CPP0_EVALUATION_H
#define SHAD_CPP0_EVALUATION_H

#endif  // SHAD_CPP0_EVALUATION_H

#include <map>
#include <functional>
#include <object.h>
#include <variant>
#include <unordered_set>

class Value;
class Scope;
class DeadFunction;
class AliveLambda;
class Lambda;
class VecObjPtr;

typedef std::unordered_map<std::string, ValuePtr> Dict;
typedef std::function<ObjPtr(Scope&, ObjPtr)> Single;
typedef std::function<ObjPtr(Scope&, ObjPtr, ObjPtr)> Double;
typedef std::function<ObjPtr(VecObjPtr,Scope&)> Many;
typedef std::variant<Single,Double, Many> DeadPrototype;

enum class ValueType;

class Scope{
public:
    explicit Scope();
    explicit Scope(Scope* previous);
    bool Has(std::shared_ptr<Symbol> symb);
    Scope* Search(std::shared_ptr<Symbol> symb);
    ValuePtr& operator[](std::shared_ptr<Symbol> symb);
    ValuePtr& operator[](std::string name);
    ValuePtr operator[](std::shared_ptr<Symbol> symb) const;
    ValuePtr operator[](std::string name) const;
    void Reset(std::shared_ptr<Symbol> symb,ValuePtr val);
    Scope New();

private:
    std::shared_ptr<Dict> scope_loc_ = std::make_shared<Dict>();
    Scope* previous_ = nullptr;
};

class Value {
public:
    virtual ~Value();
    Value();
    virtual ValuePtr operator()(Scope&,VecObjPtr);
    virtual ValueType GetType();
};

class UnDefValue : public Value {
public:
    ValueType GetType();
};

class ConstValue : public Value {
public:
    ConstValue(ObjPtr value);
    ObjPtr GetValue();
    ValueType GetType();

private:
    ObjPtr value_;
};

class AliveLambda : public Value {
public:
    AliveLambda(Scope& scope, VecObjPtr args);
    void RecursiveDefine(VecObjPtr arg_names, VecObjPtr arg);
    ValuePtr RecursiveEvaluateOperators(VecObjPtr operators);
    ValuePtr operator()(Scope&,VecObjPtr arg);
    ValueType GetType();

protected:
    Scope scope_;
    VecObjPtr arg_names_;
    VecObjPtr operators_;
};

class DeadFunction : public Value {
public:
    DeadFunction(DeadPrototype func);
    ValuePtr operator()(Scope& scope,VecObjPtr arg);
    DeadPrototype GetFunc();
    ValueType GetType();

protected:
    DeadPrototype func_;
};

class Lambda : public Value {
public:
    Lambda();
    ValuePtr operator()(Scope& scope,VecObjPtr vec);
    ValueType GetType();
};

template <class T>
std::shared_ptr<T> AsV(const ValuePtr& val) {
    return std::dynamic_pointer_cast<T>(val);

}

template <class T>
bool IsV(const ValuePtr& val) {
    return AsV<T>(val) != nullptr;
}

ObjPtr CheckConst(Scope& scope, ObjPtr obj);

ObjPtr CheckCommon(Scope& scope, ObjPtr obj);

ValuePtr ToValuePtr(ObjPtr obj);

ObjPtr ToObjPtr(ValuePtr val);

ValuePtr EvaluateObj(Scope& scope, ObjPtr obj);