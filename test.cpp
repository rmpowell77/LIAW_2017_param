#include "argo.hpp"
#include <cmath>

using namespace argo::literals;
constexpr auto default_param = "default"_arg;

int foo(int a, int b, int c) { return std::pow(a, b) * c; }

constexpr auto argspec = argo::argspec("a"_arg, "b"_arg, "c"_arg);
template <typename... Args> int kwfoo(Args&&... args)
{
  auto [arg_idx, kwarg_idx] = argo::unpack<Args...>();
//  auto collected = argo::collect(argspec, args, kwargs); // hana::map
//  auto defaulted = argo::transform(argspec, collected);  // hana::map
//  return argo::invoke_swizzled(argspec.order, defaulted, foo);
}

int main() { kwfoo(1, "c"_arg = 2, "b"_arg = 3); }

/*
def foo(*args, **kwargs):
    # body

*/
