#pragma once

#include <memory>
#include <vector>
#include <error.h>

class Value;
class Object;
class Procedure;
class Number;
class Symbol;
class Cell;
class EmptyList;
class UnDef;
class VecObjPtr;

typedef std::shared_ptr<Object> ObjPtr;
typedef std::shared_ptr<Cell> CellPtr;
typedef std::shared_ptr<Symbol> SymbPtr;
typedef std::shared_ptr<Number> NumbPtr;
typedef std::vector<ObjPtr> VecOP;
typedef std::shared_ptr<Value> ValuePtr;

class Object{
public:
    virtual ~Object();
    bool to_compute = true;
};

class Procedure : public Object {
public:
    Procedure(ValuePtr value_ptr);
    ValuePtr Get();
private:
    ValuePtr value_ptr_;
};

class Number : public Object {
public:
    Number(int number);
    int Get() const;

private:
    int number_;
};

class Symbol : public Object {
public:
    Symbol(std::string name);
    const std::string& Get() const;

private:
    std::string name_;
};

class Cell : public Object {
public:
    Cell(ObjPtr first, ObjPtr second);
    Cell(ObjPtr first);
    ObjPtr GetFirst();
    ObjPtr GetSecond();
    void ResetSecond(ObjPtr new_second);
    void ResetFirst(ObjPtr new_first);
    Object* GetUsualSecond();
    bool CheckIfSingle();
    bool CheckIfDouble();

private:
    ObjPtr first_, second_;
};

class UnDef : public Object {
public:
    UnDef();
};

class EmptyList : public Object {
public:
    EmptyList();
};

class VecObjPtr {
public:
    VecObjPtr();
    VecObjPtr(ObjPtr obj, size_t pos);
    VecObjPtr(ObjPtr obj);
    VecObjPtr(VecOP& vec, size_t pos);
    VecObjPtr Next();
    explicit operator bool() const;
    ObjPtr operator[](size_t n) const;
    size_t Size() const;

private:
    void Fill(CellPtr cell);
    VecOP vec_ = VecOP{};
    size_t pos_ = 0;
};

bool IsCell(const ObjPtr& arg);

SymbPtr FromBool(bool x);

bool IsBoolSymb(ObjPtr arg);

bool BoolFrom(ObjPtr obj);

template <class T>
std::shared_ptr<T> As(const ObjPtr& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const ObjPtr& obj) {
    return As<T>(obj) != nullptr;
}

