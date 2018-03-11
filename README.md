# LIAW_2017_param

This work was from C++Now 2017 Library in a Week.

At the start of Library in a Week 2017 we outlined a number of projects to work on.  One our group settled on was to modernize a Boost library, and we targeted [Boost Parameter](http://www.boost.org/doc/libs/1_64_0/libs/parameter/doc/html/index.html).

After several working sessions, our group came to the conclusion that Boost Parameter is a library best left unmodified.  It has a specific syntax and goal that serves it's current audience.  Instead, we started pursuing the idea of creating a new library to experiment on a core set of features we would like.

The contributors determine that while this library could be the basis of a way of having named parameters, it would be better to have this as a language feature.  This proof of concept library shows some basic functionality, but when all the complexity of overloads, templates, and Koenig lookup rules, this library may not be what users need.

This library is called ```argo``` after it's creators (Arthur, Richard, Gašper, and Odin).

## ```argo``` Parameter Goal

The intent of the library is to allow the users to use name parameters with minimal changes to their code.  For instance, we want to enable users to write functions as:

``` c++
// foo.h
int foo(int a, int b, int c);

// main.cpp
int main() {
	foo(1, "c"_arg = 3); // foo(1, 98, 3);
}
```

The idea is to have a set of parameters set by the users, default parameters, and named parameters.

To enable this functionality, you create an ```argo::argspec``` and create a new invokable object with ```argo::adapt```:

``` c++
// foo.h
#include <argo/argo.hpp>

int foo_impl(int a, int b, int c);

using namespace argo::literals;
const auto foo_argspec = argo::argspec(
    "a"_arg, // no default value
    "b"_arg = 98,
    "c"_arg = 99
);

const auto foo = argo::adapt(foo_argspec, foo_impl);

// main.cpp

int main() {
	foo(1, "c"_arg = 3); // foo(1, 98, 3);
}
```

In addition, the users may not use any named parameters, and conversions work as expected.

``` c++
int main() {
  foo(1.5); // foo(1, 98, 99);
}
```

A future library enhancement would be to add a table that retrofits an existing function to be used with named parameters.

``` c++
// foo.h
int foo(int a, int b, int c);

// main.cpp
#include <argo/argo.hpp>

using namespace argo;

CreateNameParameter_table(foo)
    "a"_arg,
    "b"_arg = 98,
    "c"_arg = 99,
End_CreateNameParameter_table()

int main() {
	foo(1, "c"_arg = 3); // foo(1, 98, 3);
}
```

By having this approach, clients would not be required to change their functions, but instead can retrofit any existing function.

A secondary goal is to allow users to create "customization points" to allow you to create parameter values based off of other parameters.

## ```argo``` Parameter Implementation Details

To explain the mechanics behind ```argo```, we will use this example:

``` c++
int foo_impl(int a, int b, int c);

using namespace argo::literals;
const auto foo_argspec = argo::argspec(
    "a"_arg, // no default value
    "b"_arg = 98,
    "c"_arg = 99
);

const auto foo = argo::adapt(foo_argspec, foo_impl);

int main() {
  foo(1, "c"_arg = 3); // foo(1, 98, 3);
}
```



### ```argo::argspec``` and parameter types

There are two types uses for parameters, ```argo::named_param```, which are named parameters with no default values (referred frequently in this doc as args) and ```argo::boxed_param```, which are named parameters with a value (what we call keyword args or kwargs).  In our example above, the User-defined literal ```_arg``` will take any string and make it into a ```named_param``` (the value ```"a"_arg``` above).  ```named_param``` overloads ```operator=``` so that the expression results in a ```boxed_param``` (the value ```"b"_arg = 98``` above).

```argspec``` returns a ```hana::tuple``` of the ```named_param``` and ```boxed_param``` for a function.  It is required that you supply a parameter for each argument to a function.  ```boxed_param``` must follow parameters that have no default value.  The supplied default value must be convertable to the argument's value.

### ```argo::adapt```

The ```argo::adapt``` function creates a lambda that when invoked will collect the arguments into a ```hana::tuple``` of values, ```named_param``` and ```box_param```.  This ```tuple``` is then parsed during Argument Resolution.

### Argument Resolution

The argument parameter resolution works as follows:

	Unpack -> NameArgs -> AddDefaults -> Collect -> Swizzle -> Apply

#### Unpack

``` c++
 * unpack: arglist -> args, kwargs
```

First, the arguments are split into two lists, the "regular" arguments (args), and the named arguments (kwargs).  For example, in above:

```
  foo(1, "c"_arg = 3) -> hana::tuple(1, "c"_arg = 3) -> { hana::tuple(1) , hana::tuple({"c", 3}) } 
```

#### NameArgs

``` c++
 * nameargs: args -> kwargs
```

We then need to give names to the "regular" arguments supplied.  We use the argspec values to assign the value arguments names in the order of the argspec.

```
  { hana::tuple(1) , hana::tuple({"c", 3}) } -> { hana::tuple({"a", 1}), hana::tuple({"c", 3}) } 
```

#### AddDefaults

We extract any kwargs from the argspec provided and append it to the tuple of kwargs.  We append it because when the hana::map of values is created, we want to supply defaults for the values that have not been supplied by the user.

```
  { hana::tuple({"a", 1}), hana::tuple({"c", 3}) } -> { hana::tuple({"a", 1}), hana::tuple({"c", 3}), hana::tuple({"b", 98}, {"c", 99}) } 
```

#### Collect

``` c++
 * collect ::  -> hana::map
 * collect: args, kwargs, ordering -> kwargs
 *   for x, v in zip(ordering, args):
 *       assert kwargs.insert(x, v).second
```

Once unpacked, the arguments are then collected into a mapping of the supplied arguments and the named parameters.

```
  { hana::tuple({"a", 1}), hana::tuple({"c", 3}), hana::tuple({"b", 98}, {"c", 99}) } -> hana::map({"a", 1}, {"c", 3}, {"b", 98})
```

#### Swizzle

``` c++
 * swizzle: ordering, hana::map -> hana::tuple
```

We then take the argspec name ordering and extract those from the map

```
  argspec -> hana::tuple("a", "b", "c");
  hana::map({"a", 1}, {"c", 3}, {"b", 98}) -> hana::tuple(1, 98, 3);
```

#### Apply

``` c++
 * apply: f, hana::tuple -> f(*hana::tuple)
```

Finally, this tuple is applied to the target function:

  hana::tuple(1, 98, 3) -> foo(1, 98, 3)


### CreateNameParameter_table

This is a possible future enhancement to the library.

The CreateNameParameter_table() macro creates a new variadic template:

``` c++
CreateNameParameter_table(foo)
    "a"_arg = no_default,
    "b"_arg = 98,
    "c"_arg = 99,
End_CreateNameParameter_table()
```

``` c++
template <typename ... Args>
auto foo(Args... args) {
...

    "a"_arg = no_default,
    "b"_arg = 98,
    "c"_arg = 99,
...
}
```

This effectively creates a new overload of ```foo()```.  When users specify all of the arguements directly, the existing ```foo()``` overloads are not called.  

The current blocking challenge is how to have ```argo::adapt``` not match against the current template function that it is defined in.  Essentially, how do we provide another overload that doesn't get matched when resolving the function to call (see below)?

``` c++
template <typename ... Args>
auto foo(Args&&... args) {
  static const auto func_argspec = argo::argspec(
  "a"_arg,
  "b"_arg = 3,
  "c"_arg = 4
  );
  static const auto adaptor = argo::adapt(func_argspec, foo); // matches against foo(...)
  adaptor(std::forward<Args>(args)...);
}
```


## Misc.

`ajo` contains a sample implementation that shows the proof of concept.
`argo` contains the outlines of code that would compose the library.

## Contributors

### Initial concept and implementation

* <i>A</i>rthur O'Dwyer
* <i>R</i>ichard Powell
* <i>G</i>ašper Ažman
* <i>O</i>din Holmes

### Additional patches, counsel, and all around awesomeness:

* Louis Dionne



