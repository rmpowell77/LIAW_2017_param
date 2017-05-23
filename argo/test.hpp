#ifndef INCLUDED_TEST_HPP
#define INCLUDED_TEST_HPP

#include "argo.hpp"

constexpr inline auto const foo_args = [] {
  using namespace argo::literals;
  return argo::argspec("a"_arg, "b"_arg, "c"_arg);
}();
int foo(int a, int b, int c);

template <typename... Args>
int kwfoo(Args&&... args) {
  return argo::invoke(foo_args, foo, std::forward<Args>(args)...);
} // kwfoo
inline auto const kwfoo2 = argo::adapt(foo_args, foo);

#endif

