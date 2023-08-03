#include <dead_functions.h>

ObjPtr IsNumber(ObjPtr arg) {
    return FromBool(Is<Number>(arg));
}

ObjPtr IsSymbol(ObjPtr arg) {
    return FromBool(Is<Symbol>(arg));
}

ObjPtr IsBoolean(ObjPtr arg) {
    return FromBool(IsBoolSymb(arg));
}

ObjPtr IsPair(ObjPtr arg) {
    return FromBool(Is<Cell>(arg) && arg);
}

ObjPtr IsList(ObjPtr arg) {
    if (!arg) {
        return FromBool(true);
    }
    if (Is<EmptyList>(arg)) {
        return FromBool(true);
    }
    if (auto cell = As<Cell>(arg)) {
        return IsList(cell->GetSecond());
    }
    return FromBool(false);
}

ObjPtr IsNull(ObjPtr arg) {
    return FromBool(Is<EmptyList>(arg));
}

ObjPtr Abs(ObjPtr arg) {
    NumbPtr numb = As<Number>(arg);
    if (!numb) {
        throw RuntimeError("It should be Number");
    }
    return std::make_shared<Number>(std::abs(numb->Get()));
}

ObjPtr Not(ObjPtr arg) {
    return FromBool(!BoolFrom(arg));
}

ObjPtr Car(ObjPtr arg) {
    CellPtr cell = As<Cell>(arg);
    if (!cell) {
        throw RuntimeError("it should be not empty Cell");
    }
    return cell->GetFirst();
}

ObjPtr Cdr(ObjPtr arg) {
    CellPtr cell = As<Cell>(arg);
    if (!cell) {
        throw RuntimeError("it should be not empty Cell");
    }
    return cell->GetSecond();
}

ObjPtr Quote(Scope&, ObjPtr arg) {
    arg->to_compute = false;
    return arg;
}

// Double Modify Functions Prototypes
// ----------------------------------------------------------------------

ObjPtr Setter(Scope& scope, ObjPtr first, ObjPtr second) {
    if (auto name = As<Symbol>(first)) {
        if (auto found = scope.Search(name)) {
            (*found)[name] = EvaluateObj(scope,second);
            return std::make_shared<UnDef>();
        }
        throw NameError("there is not such name in scope");
    }
    throw RuntimeError("incorrect type of first arg of define");
}

ObjPtr Define(Scope& scope, ObjPtr first, ObjPtr second) {
    if (auto name = As<Symbol>(first)) {
        scope[name] = EvaluateObj(scope,second);
        return std::make_shared<UnDef>();
    }
    if (auto cell = As<Cell>(first)) {
        if (auto name = As<Symbol>(cell->GetFirst())) {
            auto lambda = std::make_shared<Cell>(std::make_shared<Symbol>("lambda"));
            lambda->ResetSecond(std::make_shared<Cell>(cell->GetSecond()));
            auto tail = As<Cell>(lambda->GetSecond());
            tail->ResetSecond(std::make_shared<Cell>(second));
            scope[name] = EvaluateObj(scope,lambda);
            return std::make_shared<UnDef>();
        }
    }
    throw RuntimeError("incorrect type of arguments");
}

ObjPtr SetListPart(ObjPtr first, ObjPtr second, bool if_car) {
    if (auto list = As<Cell>(first)) {
        if (if_car) {
            list->ResetFirst(second);
        } else {
            list->ResetSecond(second);
        }
        return std::make_shared<UnDef>();
    }
    throw RuntimeError("incorrect arguments");
}
ObjPtr SetCar(ObjPtr first, ObjPtr second) {
    return SetListPart(first, second, true);
}

ObjPtr SetCdr(ObjPtr first, ObjPtr second) {
    return SetListPart(first, second, false);
}

// Double Functions
// Prototypes-----------------------------------------------------------------------
ObjPtr Cons(ObjPtr first, ObjPtr second) {
    auto ptr = std::make_shared<Cell>(first, second);
    ptr->to_compute = false;
    return ptr;
}

ObjPtr GetListPartByInt(ObjPtr first, int n, bool if_ref) {
    if (auto cell = As<Cell>(first)) {
        if (n == 0) {
            if (if_ref) {
                return cell->GetFirst();
            } else {
                return cell;
            }
        } else {
            return GetListPartByInt(cell->GetSecond(), n - 1, if_ref);
        }
    }
    if (!first && n == 0 && !if_ref) {
        return std::make_shared<EmptyList>();
    }
    throw RuntimeError("incorrect arguments");
}

ObjPtr GetListPartByNumber(ObjPtr first, ObjPtr second, bool if_ref) {
    if (auto numb = As<Number>(second)) {
        int n = numb->Get();
        if (n < 0) {
            throw RuntimeError("index should be not negative");
        }
        auto result = GetListPartByInt(first, n, if_ref);
        result->to_compute = false;
        return result;
    }
    throw RuntimeError("second argument should be number");
}

ObjPtr ListRef(ObjPtr first, ObjPtr second) {
    return GetListPartByNumber(first, second, true);
}

ObjPtr ListTail(ObjPtr first, ObjPtr second) {
    return GetListPartByNumber(first, second, false);
}

// Many Functions Prototypes------------------------------------------------------------------------

ObjPtr BinNumb(Scope& scope, VecObjPtr vec, int n, std::function<int(int, int)> bin) {
    if (!vec) {
        return std::make_shared<Number>(n);
    }
    if (auto numb = As<Number>(CheckCommon(scope, vec[0]))) {
        return BinNumb(scope, vec.Next(), bin(n, (numb->Get())), bin);
    }
    throw RuntimeError("all arguments should be numbers");
}

ObjPtr Plus(VecObjPtr vec, Scope& scope) {
    return BinNumb(scope, vec, 0, [](int x, int y) { return x + y; });
}

ObjPtr Mult(VecObjPtr vec, Scope& scope) {
    return BinNumb(scope, vec, 1, [](int x, int y) { return x * y; });
}

ObjPtr FirstObligedBinNumb(Scope& scope, VecObjPtr vec, std::function<int(int, int)> bin) {
    if (!vec) {
        throw RuntimeError("should be at least one argument");
    }
    if (auto numb = As<Number>(CheckCommon(scope,vec[0]))) {
        return BinNumb(scope, vec.Next(), numb->Get(), bin);
    }
    throw RuntimeError("all arguments should be numbers");
}

ObjPtr Minus(VecObjPtr vec, Scope& scope) {
    return FirstObligedBinNumb(scope, vec, [](int x, int y) { return x - y; });
}

ObjPtr Divide(VecObjPtr vec, Scope& scope) {
    return FirstObligedBinNumb(scope, vec, [](int x, int y) { return x / y; });
}

ObjPtr Max(VecObjPtr vec, Scope& scope) {
    return FirstObligedBinNumb(scope, vec, [](int x, int y) { return std::max(x, y); });
}

ObjPtr Min(VecObjPtr vec, Scope& scope) {
    return FirstObligedBinNumb(scope, vec, [](int x, int y) { return std::min(x, y); });
}



ObjPtr RecursiveList(Scope& scope, VecObjPtr vec, bool start) {
    if (!vec) {
        return nullptr;
    }
    auto result =
        std::make_shared<Cell>(CheckCommon(scope, vec[0]), RecursiveList(scope, vec.Next(), false));
    if (start) {
        result->to_compute = false;
    }
    return result;
}
ObjPtr List (VecObjPtr vec, Scope& scope) {
    return RecursiveList(scope,vec,true);
}

ObjPtr BinBool(Scope& scope, VecObjPtr vec, ObjPtr b,
               bool changed = false) {
    if (!vec || changed) {
        return b;
    } else {
        auto new_b = CheckCommon(scope,vec[0]);
        return BinBool(scope, vec.Next(), new_b, BoolFrom(b) != BoolFrom(new_b));
    }
}

ObjPtr And(VecObjPtr vec, Scope& scope) {
    return BinBool(scope, vec, FromBool(true));
}

ObjPtr Or(VecObjPtr vec, Scope& scope) {
    return BinBool(scope, vec, FromBool(false));
}

ObjPtr BinIntBool(Scope& scope, VecObjPtr vec, std::function<bool(int, int)> bin,
                  bool good = true, NumbPtr calculated = nullptr) {
    if (!good) {
        return FromBool(false);
    }
    if (vec.Size() <= 1) {
        return FromBool(true);
    }
    if (NumbPtr numb1 = calculated ? calculated : As<Number>(CheckCommon(scope, vec[0]))) {
        if (NumbPtr numb2 = As<Number>(CheckCommon(scope, vec[1]))) {
            return BinIntBool(scope, vec.Next(), bin, bin(numb1->Get(), numb2->Get()),numb2);
        }
    }
    throw RuntimeError("all arguments should be numbers");
}

ObjPtr Eq(VecObjPtr vec, Scope& scope) {
    return BinIntBool(scope, vec, [](int a, int b) { return a == b; });
}

ObjPtr More(VecObjPtr vec, Scope& scope) {
    return BinIntBool(scope, vec, [](int a, int b) { return a > b; });
}

ObjPtr MoreEq(VecObjPtr vec, Scope& scope) {
    return BinIntBool(scope, vec, [](int a, int b) { return a >= b; });
}

ObjPtr Less(VecObjPtr vec, Scope& scope) {
    return BinIntBool(scope, vec, [](int a, int b) { return a < b; });
}

ObjPtr LessEq(VecObjPtr vec, Scope& scope) {
    return BinIntBool(scope, vec, [](int a, int b) { return a <= b; });
}

ObjPtr If(VecObjPtr vec, Scope& scope) {
    if (vec.Size() <= 1 || vec.Size() > 3) {
        throw SyntaxError("incorrect count of arguments");
    }
    if (BoolFrom(CheckCommon(scope, vec[0]))) {
        return CheckCommon(scope, vec[1]);
    } else {
        if (vec.Size() == 2) {
            return std::make_shared<EmptyList>();
        } else {
            return CheckCommon(scope, vec[2]);
        }
    }
}

