#include "foo.hpp"
#include "argo.hpp"

#include <boost/type_index.hpp>
#include <iostream>
#include <string>
#include <cassert>


constexpr inline auto const foo_args = [] {
  using namespace argo::literals;
  return argo::argspec("a"_arg, "b"_arg, "c"_arg);
}();

inline auto const kwfoo = argo::adapt(foo_args, foo);

using namespace argo::literals;

// test is_param
static_assert(argo::is_named_param("b"_arg = 3), "");
static_assert(!argo::is_named_param(3), "");
static_assert(argo::is_not_named_param(3), "");
static_assert(!argo::is_named_param("hi"_arg), "");
static_assert(argo::is_named_param("b"_arg = 3), "");

static_assert(argo::argspec("b"_arg).names == boost::hana::make_tuple(BOOST_HANA_STRING("b")), "");
static_assert(argo::argspec("b"_arg, "aaa"_arg).names == boost::hana::make_tuple(BOOST_HANA_STRING("b"), BOOST_HANA_STRING("aaa")), "");

static_assert(boost::hana::first(decltype("b"_arg = 3)::value_type{}) == boost::hana::type<int&&>(), "");
static_assert(std::is_same_v<decltype(boost::hana::second(decltype("b"_arg = 3)::value_type())), int* const&>, "");


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
  boost::hana::for_each(answer, [&](auto x) { \
        std::cout << "<" <<type_id_with_cvr<decltype(x)>().pretty_name() <<"> "<<  x << '\n'; \
    }); \
} 

int main() { 

	PRINT_TUPLE(argo::decompose_arguments(foo_args, 1, 2, 3));
	PRINT_TUPLE(argo::decompose_arguments(foo_args, 1, 3, "c"_arg = 2));
	std::string hello = "hello";
	PRINT_TUPLE(argo::decompose_arguments(foo_args, "hi", "c"_arg = hello, "b"_arg = 2));

	PRINT(kwfoo(2, "b"_arg = 3, "c"_arg = 4));
	PRINT(kwfoo(2, "c"_arg = 4, "b"_arg = 3));
	PRINT(kwfoo("b"_arg = 3, "c"_arg = 4, "a"_arg = 2));

}
