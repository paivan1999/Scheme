---
marp: true
---

# Scheme

---

## Что такое scheme

- Язык программирования "без синтаксиса"

```scheme
42 ; constant

(+ 1 2) ; expression

(define x 3) ; variable

(define (add x y) ; function
  (+ x y))
```

---

## Почему scheme

- Хорошая тренировка в декомпозиции
- Нетривиальное использование наследования
- Хороший пример, как работать с исключениями

---

## REPL

```scheme
$ 1
> 1

$ (+ 1 2)
> 3
```

- REPL расшифровывается как MyRead, Eval, Print, Loop.
- `$` будем обозначать ввод пользователя
- `>` будем обозначать вывод интерпретатора

---

## Разделение на MyRead, Eval и Print

- *MyRead* - считывает ввод пользователя
- *Eval* - производит вычисления
- *Print* - печатает результат назад

В языке `scheme` *MyRead* и *Eval* работают с одним типом данных.

```c++
std::shared_ptr<Object> MyRead();
std::shared_ptr<Object> Eval(std::shared_ptr<Object> ptr);
void Print(std::shared_ptr<Object> ptr);
```

** _неполные сигнатуры_

---

## Что такое `Object`

```c++
class Object
    : public std::enable_shared_from_this<Object>
{
public:
    virtual ~Object() = default;

    // Other methods....
};
```

Все объекты языка представляются наследниками типа `Object`.

Наследоваться от `enable_shared_from_this` не обязательно,
но может быть полезно.

Для управления памятью мы будем использовать `shared_ptr`.

---

## Наследники `Object`

```scheme
$ 1
$ a
$ (inc 1)
```

- `Number`. Обычные числа `int64_t`.
- `Symbol`. Имена в языке называются "символами".
- `Cell` - пара. Композитный тип.

Пустой список будем представлять как nullptr.

```scheme
$ ()
```

---

## Cell

```c++
class Cell : public Object {
private:
    std::shared_ptr<Object> first_, second_;
};
```

```
$ (1 . 2) ; Пара из двух чисел

$ () ; Нулевой указатель

$ (1 . (2 . (3 . ()))) ; Список
$ (1 2 3) ; Другой синтаксис для списка

$ (1 2 3 . 4) ; Список без nullptr в конце
```

---

## Структура списков

```raw
(1 . 2)    *------*               ()
           * pair *  => 2               nullptr
           *------*
              ||
              \/
               1

(1 2)   *------*     *------*
        * pair *  => * pair * => nullptr
        *------*     *------*
           ||           ||
           \/           \/
            1            2

(1 2 . 3) *------*     *------*
          * pair *  => * pair * => 3
          *------*     *------*
             ||           ||
             \/           \/
              1            2
```

---

## Reader

Принимает на вход последовательность из `(`, `)`, `'`, `.`, чисел и символов. Возвращает один `std::shared_ptr<Object>`.

```raw
(1 2 . 3) *------*     *------*
          * pair *  => * pair * => 3
          *------*     *------*
             ||           ||
             \/           \/
              1            2
```

---

## Tokenizer

```raw
"(sum +2 . -3)"

{ "(", "sum", "+2", ".", "-3", ")" }
```

Читает байты из `std::istream*` и выдаёт поток токенов.

Токен будем хранить в `std::variant`.

```c++
struct SymbolToken {
    std::string name;
};

struct QuoteToken { };

using Token = std::variant<SymbolToken, QuoteToken>;
```

---

## Eval

```scheme
$ 1
> 1

$ (+ 1 2)
> 3

$ '(+ 1 2)
> (+ 1 2)

$ (define x 5)
> ()

$ x
> 5
```

---

## Eval функций

```scheme
$ (+ 1 (* 2 3))

~ (#<builtin-+> 1 6)

> 7
```

Функция сложения определена в С++

Символ `+` вычисляется в объект "функции".

Функция вычисляет все аргументы по обычным правилам. Затем,
запускает `C++` логику.

---

## Eval особых форм

```scheme
$ (define x 1)
> ()

$ (quote (1 2 3))
> (1 2 3)
```

Символы `quote` и `define` вычисляются в объект "особой формы".

Аргумент передаётся в форму как есть, без вычисления.

---

## Quote

```scheme
$ '(+ 1 2)
> (+ 1 2)

$ (quote (+ 1 2))
> (+ 1 2)
```

`'` - это синтаксический сахар для `(quote ...)`.

Нужно переписывать одно в другое в `Reader`.

---

## ScopePtr

`ScopePtr` хранит в себе имена.

Все встроенные имена определены в глобальном `ScopePtr`.

`define` добавляет имя в `ScopePtr`.

Вызов функции создаёт локальный `ScopePtr`.

```scheme
$ (define x 3)
$ (define (add a)
    (+ a x))

$ (add 1 2)
```

---

## Lambda capture in C++

```c++
auto Range(int start) {
  return [start] () mutable {
    ++start;
    return start;
  };
}

void F() {
  auto r = Range(10);

  std::cout << r() << std::endl; // 11
  std::cout << r() << std::endl; // 12
}
```

---

## Lambda capture

```scheme
$ (define range
    (lambda (x)
      (lambda ()
        (set! x (+ x 1))
        x)))

$ (define my-range (range 10))

$ (my-range)
> 11

$ (my-range)
> 12
```

`ScopePtr` могут быть сколь угодно вложенными.

---

## Управление памятью

Мы будем использовать `shared_ptr`.

В наивной реализации будут утечки памяти, из-за циклов по
`shared_ptr`.

Leak sanitizer отключен при тестировании на сервере, но есть
бонус на 500 баллов, где нужно реализовать сборку мусора.

---

## Приведение типов в вашем коде

```c++
template<class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj);

template<class T>
bool Is(const std::shared_ptr<Object>& obj);
```

Для тестов парсера, вам нужно определить пару хелперов `Is` и `As`. В отличии от использования `std::dynamic_pointer_cast` напрямую, вы можете задать
нужную обработку ошибок.

---

## Тестирование

```c++
class SchemeTest {
    void ExpectEq(const std::string& expression, const std::string& result) {
        REQUIRE(scheme_.Evaluated(expression) == result);
    }

    // ...

private:
    Scheme scheme_;
};

TEST_CASE_METHOD(SchemeTest, "TestCase") {
    ExpectEq("(define x 3)", "()");
    ExpectEq("x", "3");
}
```

- Создаётся один экземпляр класса `Scheme` на весь блок `TEST_CASE_METHOD`.
