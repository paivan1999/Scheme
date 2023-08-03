#pragma once
#include <evaluation.h>
#include <object.h>

// DeadFunctions Prototypes ========================================================================

// SingleCalculate Functions Prototypes
// ----------------------------------------------------------------------

ObjPtr IsNumber(ObjPtr arg);

ObjPtr IsSymbol(ObjPtr arg);

ObjPtr IsBoolean(ObjPtr arg);

ObjPtr IsPair(ObjPtr arg);

ObjPtr IsList(ObjPtr arg);

ObjPtr IsNull(ObjPtr arg);

ObjPtr Abs(ObjPtr arg);

ObjPtr Not(ObjPtr arg);

ObjPtr Car(ObjPtr arg);

ObjPtr Cdr(ObjPtr arg);

ObjPtr Quote(Scope&, ObjPtr arg);

// Double Modify Functions Prototypes
// ----------------------------------------------------------------------

ObjPtr Setter(Scope& scope, ObjPtr first, ObjPtr second);

ObjPtr Define(Scope& scope, ObjPtr first, ObjPtr second);

ObjPtr SetListPart(ObjPtr first, ObjPtr second, bool if_car);

ObjPtr SetCar(ObjPtr first, ObjPtr second);

ObjPtr SetCdr(ObjPtr first, ObjPtr second);

// Double Functions
// Prototypes-----------------------------------------------------------------------
ObjPtr Cons(ObjPtr first, ObjPtr second);

ObjPtr GetListPartByInt(ObjPtr first, int n, bool if_ref);

ObjPtr GetListPartByNumber(ObjPtr first, ObjPtr second, bool if_ref);

ObjPtr ListRef(ObjPtr first, ObjPtr second);

ObjPtr ListTail(ObjPtr first, ObjPtr second);

// Many Functions Prototypes------------------------------------------------------------------------

ObjPtr BinNumb(Scope& scope, VecObjPtr vec, int n, std::function<int(int, int)> bin);

ObjPtr Plus(VecObjPtr vec, Scope& scope);

ObjPtr Mult(VecObjPtr vec, Scope& scope);

ObjPtr FirstObligedBinNumb(Scope& scope, VecObjPtr vec, std::function<int(int, int)> bin);

ObjPtr Minus(VecObjPtr vec, Scope& scope);

ObjPtr Divide(VecObjPtr vec, Scope& scope);

ObjPtr Max(VecObjPtr vec, Scope& scope);

ObjPtr Min(VecObjPtr vec, Scope& scope);

ObjPtr RecursiveList(Scope& scope, VecObjPtr vec, bool start);

ObjPtr List (VecObjPtr vec, Scope& scope);

ObjPtr BinBool(Scope& scope, VecObjPtr vec, ObjPtr obj, std::function<bool(bool, bool)> bin,
               bool changed);

ObjPtr And(VecObjPtr vec, Scope& scope);

ObjPtr Or(VecObjPtr vec, Scope& scope);

ObjPtr BinIntBool(Scope& scope, VecObjPtr vec, std::function<bool(int, int)> bin,
                  bool good);

ObjPtr Eq(VecObjPtr vec, Scope& scope);

ObjPtr More(VecObjPtr vec, Scope& scope);

ObjPtr MoreEq(VecObjPtr vec, Scope& scope);

ObjPtr Less(VecObjPtr vec, Scope& scope);

ObjPtr LessEq(VecObjPtr vec, Scope& scope);

ObjPtr If(VecObjPtr vec, Scope& scope);
