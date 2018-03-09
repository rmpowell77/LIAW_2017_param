#ifndef INLCUDED_ARGO_HPP
#define INLCUDED_ARGO_HPP

/*
 * kwargs :: hana::map<param, {nothing, default<T>, value<T>}>;
 *
 * make_argspec: argspec_ting, [defaults, reflow spec] -> argspec
 *
 * unpack :: hana::tuple -> hana::tuple, hana::map
 * unpack: arglist -> args, kwargs
 *
 * collect ::  -> hana::map
 * collect: args, kwargs, argspec_ting -> kwargs
 *   for x, v in zip(argspec_ting, args):
 *       assert kwargs.insert(x, v).second
 *
 * transform: argspec, hana::map -> hana::map (fuller)
 * swizzle: argspec_ting, hana::map -> hana::tuple
 * apply: f, hana::tuple -> f(*hana::tuple)
 */

#include <boost/hana.hpp>

namespace argo {

template <typename Name, typename Value>
struct boxed_param {
  using pair = boost::hana::pair<Name, Value>;
  using key_type = Name;
  using value_type = Value;

  // the pair is (name, (type<T>, T*))
  pair value;
};

// trait for recognising boxed params inside the arglist
template <typename T>
struct is_boxed_param : std::false_type {};
template <typename Name, typename Value>
struct is_boxed_param<boxed_param<Name, Value>> : std::true_type {};

auto constexpr is_named_param = [](auto const& arg) {
  using T = std::decay_t<decltype(arg)>;
  return is_boxed_param<T>{};
};
auto constexpr is_not_named_param = [](auto const& arg) {
  return std::negation<decltype(is_named_param(arg))>{};
};

// how to capture parameters so we don't have to care about transport until
// perfect forwarding.
// This function is idempotent.
inline constexpr auto capture_param = [](auto&& param) constexpr {
  using boost::hana::make_pair;
  using boost::hana::type;

  if
    constexpr(decltype(is_named_param(param)){}) { return param; }
  else {
    return make_pair(type<decltype(param)>{}, std::addressof(param));
  }
};

// interface to library - this creates boxed params.
template <char... Chars>
struct named_param {
  using string = boost::hana::string<Chars...>;
  using name = string;

  template <typename T>
  constexpr auto operator=(T&& value) {
    auto const captured_param = capture_param(std::forward<T>(value));
    using return_t = boxed_param<name, decltype(captured_param)>;
    return return_t{{name{}, captured_param}};
  }
};

// keeps the order of the parameters etc.
template <typename... Args>
struct argspec_t {
  using names_t = boost::hana::tuple<typename Args::name...>;
  static constexpr names_t names = {};
};

template <typename... Args>
constexpr auto argspec(Args...) {
  return argspec_t<Args...>{};
}

// debugging tool
template <typename... _>
struct undef;

/**
 * Split the parameter pack into positional and keyword argument pointers.
 *
 * Return a boost::hana::pair of (kwargptrs, argptrs).
 *
 * @param argptrs - boost::hana::tuple of param pointers.
 */
inline auto const split_to_posargs_namedargs = [](auto type_argptr_pairs) {
  return boost::hana::span(type_argptr_pairs, is_not_named_param);
};

/**
 * Give names to positional arguments.
 */
inline auto const name_args = [](auto argnames, auto posarg_type_argptr_pairs) {
  using boost::hana::zip_shortest_with;
  using boost::hana::make_pair;

  return zip_shortest_with(make_pair, argnames, posarg_type_argptr_pairs);
};

inline auto const unbox_kwargs = [](auto kwarg_ptrs) {
  using boost::hana::transform;

  return transform(kwarg_ptrs,
                   [](auto boxed_param) { return boxed_param.value; });
};

/**
 * Deduplicate args and kwargs, and join them into a single hana map of
 * `pair<type<T>, T*>`.
 */
inline auto const collect = [](auto named_type_argptr_pairs,
                               auto named_type_kwargptr_pairs) constexpr {
  using boost::hana::concat;
  using boost::hana::to_map;
  using boost::hana::unpack;
  using boost::hana::make_map;

  return unpack(concat(named_type_argptr_pairs, named_type_kwargptr_pairs), make_map);
};

/**
 * give tuple of names for the arg order, extract values from map of name, values
 *
 * Return a boost::hana::tuple to be used with invoke.
 *
 * @param arg_order - boost::hana::tuple of names.
 * @param named_type_argptr_pairs - boost::hana::map of names to values.
 */
inline auto const swizzle_args = [](auto arg_order,
                                       auto named_type_argptr_pairs) -> decltype(auto) {
  using boost::hana::transform;

  auto params = transform(arg_order, [&](auto const& key) -> decltype(auto) {
    using boost::hana::first;
    using boost::hana::second;

    auto const type_argptr_pair = named_type_argptr_pairs[key];
    using argtype = typename decltype(+first(type_argptr_pair))::type;
    return static_cast<argtype>(*second(type_argptr_pair));
  });
  return params;
};

/**
 * Given an argspec and a set of position/kw arguments, decompose to the function arguments.
 *
 * Return a boost::hana::tuple to be used with invoke.
 *
 * @param argptrs - argspec_t for a function.
 * @param fargs - arguments
 */
inline auto const decompose_arguments = [](auto argspec, auto&&... fargs) {
  using boost::hana::make_tuple;
  using boost::hana::first;
  using boost::hana::second;

  auto const args_all_gathered =
      make_tuple(capture_param(static_cast<decltype(fargs)>(fargs))...);
  auto const type_arg_pairs_and_boxed_params =
      split_to_posargs_namedargs(args_all_gathered);
  auto const named_type_argptr_pairs =
      name_args(argspec.names, first(type_arg_pairs_and_boxed_params));
  auto const named_type_kwargptr_pairs =
      unbox_kwargs(second(type_arg_pairs_and_boxed_params));
  auto const collected_named_type_argptr_pairs =
      collect(named_type_argptr_pairs, named_type_kwargptr_pairs);
  auto const extracted_args = 
      swizzle_args(argspec.names, collected_named_type_argptr_pairs);
  return extracted_args;
};

inline auto const invoke = [](auto argspec, auto&& f, auto&&... fargs) {
  using boost::hana::unpack;

  auto const extracted_args =  decompose_arguments(argspec, fargs...);
  return unpack(extracted_args, f);
};

inline auto const adapt = [](auto&& argspec, auto&& f) {
  return [
    argspec = static_cast<decltype(argspec)>(argspec),
    f = static_cast<decltype(f)>(f)
  ](auto&&... args)
      ->decltype(auto) {
    return invoke(argspec, f, static_cast<decltype(args)>(args)...);
  };
};

namespace literals {
template <typename CharT, CharT... Chars>
constexpr auto operator"" _arg() -> named_param<Chars...> {
  return {};
}
} // namespace literals

} // namespace argo

#endif
