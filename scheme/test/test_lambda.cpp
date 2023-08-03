#include <test/scheme_test.h>

TEST_CASE_METHOD(SchemeTest, "SimpleLambda") {
    ExpectEq("((lambda (x) (+ 1 x)) 5)", "6");
}

TEST_CASE_METHOD(SchemeTest, "LambdaBodyHasImplicitBegin") {
    ExpectNoError("(define test (lambda (x) (set! x (* x 2)) (+ 1 x)))");
    ExpectEq("(test 20)", "41");
}

TEST_CASE_METHOD(SchemeTest, "SlowSum") {
    ExpectNoError("(define slow-add (lambda (x y) (if (= x 0) y (slow-add (- x 1) (+ y 1)))))");
    ExpectEq("(slow-add 1 1)", "2");
    ExpectEq("(slow-add 100 100)", "200");
}

TEST_CASE_METHOD(SchemeTest, "LambdaClosure") {
//    Scheme scheme = Scheme();
//    Scope glob = scheme.CreateGlobal();
//    auto q = Define(glob,std::make_shared<Symbol>("x"), std::make_shared<Number>(10));
//    auto args_x = std::make_shared<Cell>(std::make_shared<Cell>(std::make_shared<Symbol>("x")));
//    auto lambda_in = std::make_shared<Cell>(std::make_shared<Symbol>("lambda"));
//    auto empty = std::make_shared<Cell>(nullptr);
//    empty->ResetSecond(std::make_shared<Cell>(std::make_shared<Number>(2)));
//    lambda_in->ResetSecond(empty);
//    args_x->ResetSecond(std::make_shared<Cell>(lambda_in));
//    auto lambda_out = std::make_shared<Cell>(std::make_shared<Symbol>("lambda"),args_x);
//    auto v = Define(glob,std::make_shared<Symbol>("test"),lambda_out);
//    auto result = std::make_shared<Cell>(std::make_shared<Symbol>("test"));
//    result->ResetSecond(std::make_shared<Cell>(std::make_shared<Number>(3)));
//    auto d = Define(glob, std::make_shared<Symbol>("result"),result);
//    EvaluateObj(glob,std::make_shared<Cell>(std::make_shared<Symbol>("result")));
    ExpectNoError("(define x 1)");

    // R"EOF( is not part of the string. It is syntax for raw string literal in C++.
    // https://en.cppreference.com/w/cpp/language/string_literal
    ExpectNoError(R"EOF(
        (define range (lambda (x) (lambda () (set! x (+ x 1)) x)))
                    )EOF");

    ExpectNoError("(define my-range (range 10))");
    ExpectEq("(my-range)", "11");
    ExpectEq("(my-range)", "12");
    ExpectEq("(my-range)", "13");

    ExpectEq("x", "1");
}

TEST_CASE_METHOD(SchemeTest, "LambdaSyntax") {
    ExpectSyntaxError("(lambda)");
    ExpectSyntaxError("(lambda x)");
    ExpectSyntaxError("(lambda (x))");
}

TEST_CASE_METHOD(SchemeTest, "DefineLambdaSugar") {
//    Scheme scheme;
//    auto glob = scheme.CreateGlobal();
//    auto obj1 = std::make_shared<Cell>(std::make_shared<Symbol>("inc"));
//    obj1->ResetSecond(std::make_shared<Cell>(std::make_shared<Symbol>("x")));
//    auto obj2 = std::make_shared<Cell>(std::make_shared<Symbol>("+"));
//    auto one = std::make_shared<Cell>(std::make_shared<Number>(1));
//    auto x = std::make_shared<Cell>(std::make_shared<Symbol>("x"),one);
//    obj2->ResetSecond(x);
//    auto v = Define(glob,obj1,obj2);
//    auto inc = std::make_shared<Cell>(std::make_shared<Symbol>("inc"));
//    inc->ResetSecond(std::make_shared<Cell>(std::make_shared<Number>(-1)));
//    auto q = EvaluateObj(glob,inc);
    ExpectNoError("(define (inc x) (+ x 1))");
    ExpectEq("(inc -1)", "0");

    ExpectNoError("(define (add x y) (+ x y 1))");
    ExpectEq("(add -10 10)", "1");

    ExpectNoError("(define (zero) 0)");
    ExpectEq("(zero)", "0");
}
