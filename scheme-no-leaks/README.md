# scheme-no-leaks

**NOTE:** Исходный код этой задачи находится в директории `scheme/`.
Для сдачи задания поменяйте содержимое этой директории (например, создав пустой файл)
и отправте эти изменения в свой удалённый репозиторий.

Вам необходимо избравиться от утечек памяти в задаче scheme.
Причиной утечек является наличие циклов в графе объектов std::shared_ptr

```c++
#include <memory>
#include <iostream>

struct X {
    std::shared_ptr<X> x;
};

void Foo() {
    auto a = std::make_shared<X>();
    auto b = std::make_shared<X>();
    a->x = b;
    b->x = a;
    std::cout << a.use_count() << ' ' << b.use_count() << '\n';
} // memory leak
```
