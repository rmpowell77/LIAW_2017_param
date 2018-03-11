#include "foo.hpp"
#include "argo.hpp"

#include <boost/type_index.hpp>
#include <iostream>
#include <string>
#include <cassert>


using boost::typeindex::type_id_with_cvr;

using namespace argo::literals;

constexpr inline auto const foo_args = [] {
  using namespace argo::literals;
  return argo::argspec("a"_arg, "b"_arg, "c"_arg);
}();


const auto foo_args2 = argo::argspec("a"_arg, "b"_arg, "c"_arg = 5);

inline auto const kwfoo = argo::adapt(foo_args, foo);
inline auto const kwfoo2 = argo::adapt(foo_args2, foo);



#define PRINT(A) \
{ \
  std::cout<<#A<<"\n"; \
  auto answer = A; \
  std::cout<<answer<<" = "<<#A<<"\n"; \
}

#define PRINT_TUPLE(A) \
{ \
	using boost::typeindex::type_id_with_cvr; \
  std::cout<<#A<<"\n"; \
  auto answer = A; \
  boost::hana::for_each(answer, [](auto&& x) { \
        std::cout << "<" <<type_id_with_cvr<decltype(x)>().pretty_name() <<"> "<<  x << '\n'; \
    }); \
} 


int foo2(int a, int b, int c) {
  std::cout<<a<<","<<b<<","<<c<<"\n";
  return 1;
}

int foo3(int a, int b, int c) {
  std::cout<<a<<","<<b<<","<<c<<"\n";
  return 1;
}

int main() { 

	PRINT_TUPLE(argo::details::decompose_arguments(foo_args, 1, 2, 3));
	PRINT_TUPLE(argo::details::decompose_arguments(foo_args, 1, 3, "c"_arg = 2));
	std::string hello = "hello";
	PRINT_TUPLE(argo::details::decompose_arguments(foo_args, "hi", "c"_arg = hello, "b"_arg = 2));

  PRINT_TUPLE(argo::details::decompose_arguments(foo_args2, 2, 3));
  PRINT_TUPLE(argo::details::decompose_arguments(foo_args2, 2, 3, 4));

	PRINT(kwfoo2(2, 3));
  PRINT(kwfoo2(2, 3, 4));
	PRINT(kwfoo(2, "c"_arg = 4, "b"_arg = 3));
	PRINT(kwfoo("b"_arg = 3, "c"_arg = 4, "a"_arg = 2));

}
