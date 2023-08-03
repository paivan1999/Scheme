#include <memory>
#include <vector>
#include <error.h>
#include <object.h>

Object::~Object() = default;

Procedure::Procedure(ValuePtr value_ptr) : value_ptr_(value_ptr) {
    to_compute = false;
}
ValuePtr Procedure::Get() {
    return value_ptr_;
}

Number::Number(int number) : number_(number) {
    to_compute = false;
}
int Number::Get() const {
    return number_;
}

Symbol::Symbol(std::string name) :name_(name){
    if (name_ == "#t" || name_ == "#f") {
        to_compute = false;
    }
}
const std::string& Symbol::Get() const {
    return name_;
}

Cell::Cell(ObjPtr first):first_(first),second_(nullptr) {}
Cell::Cell(ObjPtr first, ObjPtr second): first_(first),second_(second) {}
ObjPtr Cell::GetFirst() {
    return first_;
}
ObjPtr Cell::GetSecond() {
    return second_;
}
void Cell::ResetSecond(ObjPtr new_second) {
    second_ = new_second;
}
void Cell::ResetFirst(ObjPtr new_first) {
    first_ = new_first;
}
bool Cell::CheckIfSingle() {
    return !(this->GetSecond());
}
bool Cell::CheckIfDouble() {
    CellPtr tail = As<Cell>(this->GetSecond());
    if (!tail) {
        return false;
    }
    if (tail->GetSecond()) {
        return false;
    }
    return true;
}
Object* Cell::GetUsualSecond() {
    return second_.get();
}

UnDef::UnDef() {
    to_compute = false;
}

EmptyList::EmptyList() {
    to_compute = false;
}

VecObjPtr::VecObjPtr(){}
VecObjPtr::VecObjPtr(ObjPtr obj, size_t pos) : pos_(pos) {
    Fill(As<Cell>(obj));
}
VecObjPtr::VecObjPtr(ObjPtr obj) : VecObjPtr(obj, 0){}
VecObjPtr::VecObjPtr(VecOP& vec, size_t pos) : vec_(vec), pos_(pos){}
VecObjPtr VecObjPtr::Next() {
    return VecObjPtr(vec_, pos_ + 1);
}
VecObjPtr::operator bool() const {
    return Size() > 0;
}
ObjPtr VecObjPtr::operator[](size_t n) const {
    return vec_[pos_ + n];
}
size_t VecObjPtr::Size() const{
    return vec_.size() - pos_;
}
void VecObjPtr::Fill(CellPtr cell) {
    if (!cell) {
        return;
    }
    if (!IsCell(cell->GetSecond())) {
        throw std::runtime_error("arguments should be usual list");
    }
    vec_.push_back(cell->GetFirst());
    Fill(As<Cell>(cell->GetSecond()));
}

SymbPtr FromBool(bool x) {
    if (x) {
        return std::make_shared<Symbol>("#t");
    } else {
        return std::make_shared<Symbol>("#f");
    }
}

bool IsCell(const ObjPtr& obj) {
    return !obj || As<Cell>(obj);
}

bool IsBoolSymb(ObjPtr arg) {
    SymbPtr symb = As<Symbol>(arg);
    return symb && (symb->Get() == "#t" || symb->Get() == "#f");
}

bool BoolFrom(ObjPtr obj) {
    if (auto symb = As<Symbol>(obj)) {
        if (symb->Get() == "#f") {
            return false;
        }
    }
    return true;
}
