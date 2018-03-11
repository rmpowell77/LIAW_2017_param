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

using namespace argo::literals;

// test is_param
static_assert(argo::is_boxed_param_helper("b"_arg = 3), "");
static_assert(!argo::is_boxed_param_helper(3), "");
static_assert(argo::is_not_boxed_param_helper(3), "");
static_assert(!argo::is_boxed_param_helper("hi"_arg), "");
static_assert(argo::is_boxed_param_helper("b"_arg = 3), "");

// static_assert(argo::argspec("b"_arg).names == boost::hana::make_tuple(BOOST_HANA_STRING("b")), "");
// static_assert(argo::argspec("b"_arg, "aaa"_arg).names == boost::hana::make_tuple(BOOST_HANA_STRING("b"), BOOST_HANA_STRING("aaa")), "");

// static_assert(boost::hana::first(decltype("b"_arg = 3)::value_type{}) == boost::hana::type<int&&>(), "");
// static_assert(std::is_same_v<decltype(boost::hana::second(decltype("b"_arg = 3)::value_type())), int* const&>, "");


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

constexpr inline auto const my_swizzle_args = [](auto arg_order,
                                       auto named_type_argptr_pairs) -> decltype(auto) {
  using boost::hana::transform;

   auto params = transform(arg_order, [&](auto const& key) -> decltype(auto) {
    std::cout<<"looking up key "<<type_id_with_cvr<decltype(key)>().pretty_name()<<"\n";
    using boost::hana::first;
    using boost::hana::second;

    auto const type_argptr_pair = named_type_argptr_pairs[key];
    std::cout<<"type_argptr_pair "<<type_id_with_cvr<decltype(type_argptr_pair)>().pretty_name()<<"\n";
    using argtype = typename decltype(+first(type_argptr_pair))::type;
    std::cout<<"argtype "<<type_id_with_cvr<argtype>().pretty_name()<<"\n";
    std::cout<<"pointer is "<<second(type_argptr_pair)<<"\n";
    return static_cast<std::decay_t<argtype>>(second(type_argptr_pair));
  });
  return params;
};

inline auto const my_collect = [](auto named_argspec_pairs, auto named_type_argptr_pairs,
                               auto named_type_kwargptr_pairs) constexpr {
  using boost::hana::concat;
  using boost::hana::to_map;
  using boost::hana::unpack;
  using boost::hana::make_map;

  return unpack(concat(concat(named_argspec_pairs, named_type_argptr_pairs), named_type_kwargptr_pairs), make_map);
};



inline auto const argspec2_names = [](auto&& argspec) {
  using boost::hana::transform;
    using boost::hana::first;
  using boost::hana::make_tuple;
    using boost::hana::second;
        // std::cout << "argspec = <" <<type_id_with_cvr<decltype(argspec)>().pretty_name() <<">\n\n";

  auto const extracted_value =
      argo::split_to_posargs_namedargs(argspec);
        // std::cout << "first extracted_value = <" <<type_id_with_cvr<decltype(first(extracted_value))>().pretty_name() <<">\n\n";
        // std::cout << "second extracted_value = <" <<type_id_with_cvr<decltype(second(extracted_value))>().pretty_name() <<">\n\n";

  auto const first_names = transform(first(extracted_value),
                   [](auto&& named_param) { return named_param.get_name(); });
        // std::cout << "first_names = <" <<type_id_with_cvr<decltype(first_names)>().pretty_name() <<">\n\n";

  auto const second_names = transform(second(extracted_value),
                   [](auto boxed_param) { return first(boxed_param.value); });

        // std::cout << "second_names = <" <<type_id_with_cvr<decltype(second_names)>().pretty_name() <<">\n\n";
    auto const arg_names = boost::hana::concat(first_names, second_names);
        // std::cout << "arg_names = <" <<type_id_with_cvr<decltype(arg_names)>().pretty_name() <<">\n\n";

    return arg_names;
};




inline auto const my_decompose_arguments = [](auto argspec, auto&&... fargs) {
  using boost::hana::make_tuple;
  using boost::hana::first;
  using boost::hana::second;
  using boost::hana::concat;


// gather up the names
        std::cout << "argspec = <" <<type_id_with_cvr<decltype(argspec)>().pretty_name() <<">\n\n";

  auto const arg_names = argo::argspec_extract_names(argspec);
        std::cout << "arg_names = <" <<type_id_with_cvr<decltype(arg_names)>().pretty_name() <<">\n\n";

  auto const argspec_supplied_values =
      argo::unbox_kwargs(second(argo::split_to_posargs_namedargs(argspec)));
        std::cout << "argspec_supplied_values = <" <<type_id_with_cvr<decltype(argspec_supplied_values)>().pretty_name() <<">\n\n";


  auto const args_all_gathered =
      make_tuple(argo::capture_param(static_cast<decltype(fargs)>(fargs))...);
        std::cout << "args_all_gathered = <" <<type_id_with_cvr<decltype(args_all_gathered)>().pretty_name() <<">\n\n";
  auto const type_arg_pairs_and_boxed_params =
      argo::split_to_posargs_namedargs(args_all_gathered);
        std::cout << "type_arg_pairs_and_boxed_params = <" <<type_id_with_cvr<decltype(type_arg_pairs_and_boxed_params)>().pretty_name() <<">\n\n";
  auto const named_type_argptr_pairs =
      argo::name_args(arg_names, first(type_arg_pairs_and_boxed_params));
        std::cout << "named_type_argptr_pairs = <" <<type_id_with_cvr<decltype(named_type_argptr_pairs)>().pretty_name() <<">\n\n";
  auto const named_type_kwargptr_pairs =
      argo::unbox_kwargs(second(type_arg_pairs_and_boxed_params));
        std::cout << "named_type_kwargptr_pairs = <" <<type_id_with_cvr<decltype(named_type_kwargptr_pairs)>().pretty_name() <<">\n\n";

// //  auto const collected_named_type_argptr_pairs1 =
// //      argo::collect(second(split_argspec), named_type_argptr_pairs);
// //        std::cout << "collected_named_type_argptr_pairs1 = <" <<type_id_with_cvr<decltype(collected_named_type_argptr_pairs1)>().pretty_name() <<">\n\n";

  // std::cout << "collecting  = <" <<type_id_with_cvr<decltype(argspec_supplied_values)>().pretty_name() <<">\n\n";
  // std::cout << "collecting  = <" <<type_id_with_cvr<decltype(named_type_argptr_pairs)>().pretty_name() <<">\n\n";
  // std::cout << "collecting  = <" <<type_id_with_cvr<decltype(named_type_kwargptr_pairs)>().pretty_name() <<">\n\n";
  // auto const collected_fully = concat(concat(argspec_supplied_values, named_type_argptr_pairs), named_type_kwargptr_pairs);
  // std::cout << "becomes  = <" <<type_id_with_cvr<decltype(collected_fully)>().pretty_name() <<">\n\n";

  using boost::hana::concat;
  using boost::hana::to_map;
  using boost::hana::unpack;
  using boost::hana::make_map;

   std::cout << "first collection = <" <<type_id_with_cvr<decltype(concat(concat(named_type_argptr_pairs, named_type_kwargptr_pairs), argspec_supplied_values))>().pretty_name() <<">\n\n";
  auto const new_map =
      boost::hana::to<boost::hana::map_tag>(concat(concat(named_type_argptr_pairs, named_type_kwargptr_pairs), argspec_supplied_values));
        std::cout << "new_map = <" <<type_id_with_cvr<decltype(new_map)>().pretty_name() <<">\n\n";

  auto const collected_named_type_argptr_pairs =
      unpack(concat(named_type_argptr_pairs, named_type_kwargptr_pairs), make_map);
        std::cout << "collected_named_type_argptr_pairs = <" <<type_id_with_cvr<decltype(collected_named_type_argptr_pairs)>().pretty_name() <<">\n\n";


//   auto const extracted_args =
//       argo::swizzle_args(arg_names, collected_named_type_argptr_pairs);
//         std::cout << "extracted_args = <" <<type_id_with_cvr<decltype(extracted_args)>().pretty_name() <<">\n\n";
  // return extracted_args;
  using boost::hana::transform;

  auto params = transform(arg_names, [&](auto&& key) -> decltype(auto) {
    using boost::hana::first;
    using boost::hana::second;
        std::cout << "key = <" <<type_id_with_cvr<decltype(key)>().pretty_name() <<">\n\n";


   // auto const type_argptr_pair = collected_named_type_argptr_pairs[key];
    // using argtype = typename decltype(+first(type_argptr_pair))::type;
    return 1; //static_cast<std::decay_t<argtype>>(second(type_argptr_pair));
//    return static_cast<argtype>(*second(type_argptr_pair));
  });
//  return params;

return 1;
};

inline auto const walk_args = [](auto&&... fargs) {
  using boost::hana::transform;
    using boost::hana::first;
  using boost::hana::make_tuple;
    using boost::hana::second;
  auto const args_all_gathered =
      make_tuple(argo::capture_param(static_cast<decltype(fargs)>(fargs))...);

        std::cout << "args_all_gathered = <" <<type_id_with_cvr<decltype(args_all_gathered)>().pretty_name() <<">\n\n";

  auto const extracted_value = transform(args_all_gathered,
                   [](auto boxed_param) { return second(boxed_param.value); });

        std::cout << "extracted_value = <" <<type_id_with_cvr<decltype(extracted_value)>().pretty_name() <<">\n\n";

  // boost::hana::for_each(extracted_value, [&](auto x) {
  //   using argtype = typename decltype(+first(x))::type;
  //   std::cout << "pointer is "<<second(x) <<"\n";
  //   });

};

inline auto const walk_argspec = [](auto&& argspec) {
  using boost::hana::transform;
    using boost::hana::first;
  using boost::hana::make_tuple;
    using boost::hana::second;
        std::cout << "argspec = <" <<type_id_with_cvr<decltype(argspec)>().pretty_name() <<">\n\n";

  auto const extracted_value =
      argo::split_to_posargs_namedargs(argspec);
        std::cout << "first extracted_value = <" <<type_id_with_cvr<decltype(first(extracted_value))>().pretty_name() <<">\n\n";
        std::cout << "second extracted_value = <" <<type_id_with_cvr<decltype(second(extracted_value))>().pretty_name() <<">\n\n";

  auto const first_names = transform(first(extracted_value),
                   [](auto&& named_param) { return named_param.get_name(); });
        std::cout << "first_names = <" <<type_id_with_cvr<decltype(first_names)>().pretty_name() <<">\n\n";

  auto const second_names = transform(second(extracted_value),
                   [](auto boxed_param) { return first(boxed_param.value); });

        std::cout << "second_names = <" <<type_id_with_cvr<decltype(second_names)>().pretty_name() <<">\n\n";
    auto const arg_names = boost::hana::concat(first_names, second_names);
        std::cout << "arg_names = <" <<type_id_with_cvr<decltype(arg_names)>().pretty_name() <<">\n\n";

  // boost::hana::for_each(extracted_value2, [&](auto x) {
  //   using argtype = typename decltype(+first(x))::type;
  //   std::cout << "pointer is "<<second(x) <<"\n";
  //   });

};


int foo2(int a, int b, int c) {
  std::cout<<a<<","<<b<<","<<c<<"\n";
  return 1;
}



int main() { 

//	PRINT_TUPLE(argo::decompose_arguments(foo_args, 1, 2, 3));
	// PRINT_TUPLE(my_decompose_arguments(foo_args, 1, 3, "c"_arg = 2));
//	std::string hello = "hello";
//	PRINT_TUPLE(argo::decompose_arguments(foo_args, "hi", "c"_arg = hello, "b"_arg = 2));

 // walk_args("b"_arg = 3, "c"_arg = 4, "a"_arg = 2);
 walk_argspec(foo_args2);
namespace hana = boost::hana;
using namespace std::literals;

  auto const my_map = hana::make_map(
            hana::make_pair(hana::int_c<1>, "foobar"s),
            hana::make_pair(hana::type_c<void>, 1234)
        );

 BOOST_HANA_RUNTIME_CHECK(
        my_map
        ==
        hana::make<hana::map_tag>(
            hana::make_pair(hana::int_c<1>, "foobar"s),
            hana::make_pair(hana::type_c<void>, 1234)
        )
    );
  auto const my_map2 = hana::insert(my_map, hana::make_pair(hana::int_c<1>, "barbaz"s));

 BOOST_HANA_RUNTIME_CHECK(
        my_map2
        ==
        hana::make<hana::map_tag>(
            hana::make_pair(hana::int_c<1>, "foobar"s),
//            hana::make_pair(hana::int_c<2>, "barbaz"s),
            hana::make_pair(hana::type_c<void>, 1234)
        )
    );

  (my_decompose_arguments(foo_args2, 2, 3));
  (my_decompose_arguments(foo_args2, 2, 3, 4));
  PRINT_TUPLE(argo::decompose_arguments(foo_args2, 2, 3));
  PRINT_TUPLE(argo::decompose_arguments(foo_args2, 2, 3, 4));

 //  auto const extracted_args =  argo::decompose_arguments(foo_args2, 2, 3);

	PRINT(kwfoo2(2, 3));
  PRINT(kwfoo2(2, 3, 4));
//	PRINT(kwfoo(2, "c"_arg = 4, "b"_arg = 3));
//	PRINT(kwfoo("b"_arg = 3, "c"_arg = 4, "a"_arg = 2));

}
