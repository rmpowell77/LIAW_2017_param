#include "argo.hpp"

#include <iostream>

void foo_impl(int a, int b, int c) {
  std::cout<<"foo("<<a<<", "<<b<<", "<<c<<");\n";
}

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

