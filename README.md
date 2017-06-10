# LIAW_2017_param

This work was from C++Now 2017 Library in a Week.

At the start of Library in a Week 2017 we outlined a number of projects to work on.  One our group settled on was to modernize a Boost library, and we targeted [Boost Parameter](http://www.boost.org/doc/libs/1_64_0/libs/parameter/doc/html/index.html).

After several working sessions, our group came to the conclusion that Boost Parameter is a library best left unmodified.  It has a specific syntax and goal that serves it's current audience.  Instead, we started pursuing the idea of creating a new library to experiment on a core set of features we would like.

The contributors determine that while this library could be the basis of a way of having named parameters, it would be better to have this as a language feature.  This proof of concept library shows some basic functionality, but when all the complexity of overloads, templates, and Koenig lookup rules, this library may not be what users need.

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

To enable this functionality, users have two choices.  If they would like to rewrite a new flexible API, they can use argo directly. 

``` c++
// foo.h
int foo_impl(int a, int b, int c);

constexpr auto foo_argspec = argo::argspec(
    "a"_arg = nodefault(int),
    "b"_arg = 98,
    "c"_arg = 99,
);
template <typename... Args> int foo(Args&&... args)
{
    return argo::invoke_with_args(foo_impl, args);
}


// main.cpp

int main() {
	foo(1, "c"_p = 3); // foo(1, 98, 3);
}

```

A second way would be to add a table that retrofits an existing function to be used with named parameters.

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

The argument parameter resolution works as follows:

	Unpack -> Collect -> Transform -> Swizzle -> Apply

#### Unpack

``` c++
 * unpack :: hana::tuple -> hana::tuple, hana::map
 * unpack: arglist -> args, kwargs
```

First, the arguments are unpacked from a hana::tuple to a { hana::tuple, hana::map }.  This step is to separate the supplied arguments into a list of users supplied regular arguments, and a list of user supplied named arugments.

  arg(1, "c"_p = 3) -> hana::tuple(1, "c"_p = 3) -> { hana::tuple(1) , hana::map({"c"_p, 3}) } 

#### Collect

``` c++
 * collect ::  -> hana::map
 * collect: args, kwargs, ordering -> kwargs
 *   for x, v in zip(ordering, args):
 *       assert kwargs.insert(x, v).second
```

Once unpacked, the arguments are then collected into a mapping of the supplied arguments and the named parameters.

#### Transform

``` c++
 * transform: argspec, hana::map -> hana::map (fuller)
```

We then transform the map and add default values, and new named parameters, and other itmes.

  hana::map({0, 1}, {1, 98}, { 2, 3 }};

#### Swizzle

``` c++
 * swizzle: ordering, hana::map -> hana::tuple
```

Then finally we unroll into a tuple.

  hana::map({0, 1}, {1, 98}, { 2, 3 }) -> hana::tuple(1, 98, 3);

#### Apply

``` c++
 * apply: f, hana::tuple -> f(*hana::tuple)
```

Finally, this tuple is applied to the target function:

  hana::tuple(1, 98, 3) -> foo(1, 98, 3)


### CreateNameParameter_table

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

## Misc.

`ajo` contains a sample implementation that shows the proof of concept.
`argo` contains the outlines of code that would compose the library.

## Contributors

### Initial concept and implementation

* _A_rthur O'Dwyer
* _R_ichard Powell
* _G_ašper Ažman
* _O_din Holmes

### Additional patches, counsel, and all around awesomeness:

* Louis Dionne



