# LIAW_2017_param

This work was from C++Now 2017 Library in a Week.

At the start of Library in a Week 2017 we outlined a number of projects to work on.  One our group settled on was to modernize a Boost library, and we targeted [Boost Parameter](http://www.boost.org/doc/libs/1_64_0/libs/parameter/doc/html/index.html).

After several working sessions, our group came to the conclusion that Boost Parameter is a library best left unmodified.  It has a specific syntax and goal that serves it's current audience.  Instead, we started pursuing the idea of creating a new library to experiment on a core set of features we would like.

This library is called ARGO after it's creators (Arthur, Richard, Gašper, and Odin).

## ARGO Parameter Goal

The intent of the library is to allow the users to use name parameters with minimal changes to their code.  For instance, we want to enable users to write functions as:

``` c++
// foo.h
int foo(int a, int b, int c);

// main.cpp
int main() {
	foo(1, "c"_p = 3); // foo(1, 98, 3);
}
```

The idea is to have a set of parameters set by the users, default parameters, and named parameters.

To enable this functionality, the user add a table that describes the parameters:

``` c++
// foo.h
int foo(int a, int b, int c);

// main.cpp
#include <argo/parameter.hpp>

using namespace argo;

CreateNameParameter_table(foo)
    "a"_p = no_default,
    "b"_p = 98,
    "c"_p = 99,
End_CreateNameParameter_table()

int main() {
	foo(1, "c"_p = 3); // foo(1, 98, 3);
}
```

The user creates a table that describes the named parameters, and can specify that the parameters have no defaults or a set default, very similar to Python style named parameters.

In addition, the users may not use any named parameters, and conversions work as expected.

``` c++
int main() {
	foo(1.5); // foo(1, 98, 99);
}
```

By having this approach, clients would not be required to change their functions, but instead can retrofit any existing function.

A secondary goal is to allow users to create "customization points" to allow you to create parameter values based off of other parameters.

## ARGO Parameter Implementation details

The CreateNameParameter_table() macro creates a new variadic template:

``` c++
CreateNameParameter_table(foo)
    "a"_p = no_default,
    "b"_p = 98,
    "c"_p = 99,
End_CreateNameParameter_table()
```

``` c++
template <typename Args...>
auto foo(Args... args) {
...

    "a"_p = no_default,
    "b"_p = 98,
    "c"_p = 99,
...
}
```

This effectively creates a new overload of ```foo()```.  When users specify all of the arguements directly, the existing ```foo()``` overloads are not called.  


### Contributors

	* Arthur O'Dwyer
	* Richard Powell
	* Gašper Ažman
	* Odin Holmes

ARGO!

