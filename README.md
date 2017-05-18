# LIAW_2017_param

## Goal



``` c++
// foo.h
int foo(int a, int b, int c);

// main.cpp
int main() {
	foo(1, "c"_p = 3, "b"_p = 2); // foo(1, 2, 3);
}
```

Minimal impact to the user to use this.

``` c++
// foo.h
int foo(int a, int b, int c);

// main.cpp
#include <ogar/parameter.hpp>

using namespace ogar;

CreateNameParameters(
    "a"_p = no_default,
    "b"_p = 98,
    "c"_p = 99,
)

int main() {
	foo(1, "c"_p = 3, "b"_p = 2); // foo(1, 2, 3);
	foo(1.5); // foo(1, 98, 99);
}
```



### Contributors

Odin Holmes
Gašper Ažman
Arthur O'Dwyler
Richard Powell

OGAR!

