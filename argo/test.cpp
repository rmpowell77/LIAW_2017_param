#include "argo.hpp"
#include <cmath>
#include <iostream>

using namespace argo::literals;
constexpr auto default_param = "default"_arg;

int foo(int a, int b, int c) { return std::pow(a, b) * c; }

constexpr auto argspec = argo::argspec("a"_arg, "b"_arg, "c"_arg);
template <typename... Args>
int kwfoo(Args &&... args) {
  auto const arg_kwarg_idxes = argo::unpack<Args...>();
  auto const arg_idx         = boost::hana::first(arg_kwarg_idxes);
  auto const kwarg_idx       = boost::hana::second(arg_kwarg_idxes);
  auto const collected       = argo::collect(argspec.names, arg_idx, kwarg_idx,
                                       &args...);       // hana::map
  auto defaulted = argo::transform(argspec, collected); // hana::map
  return argo::invoke_swizzled(argspec.names, defaulted, foo);
}

int main() { std::cout << kwfoo(2, "c"_arg = 3, "b"_arg = 4) << "\n"; }

/*
   def foo(*args, **kwargs):
# body

*/
