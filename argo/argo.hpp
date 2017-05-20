/*
 * kwargs :: hana::map<param, {nothing, default<T>, value<T>}>;
 *
 * make_argspec: ordering, [defaults, reflow spec] -> argspec
 *
 * unpack :: hana::tuple -> hana::tuple, hana::map
 * unpack: arglist -> args, kwargs
 *
 * collect ::  -> hana::map
 * collect: args, kwargs, ordering -> kwargs
 *   for x, v in zip(ordering, args):
 *       assert kwargs.insert(x, v).second
 *
 * transform: argspec, hana::map -> hana::map (fuller)
 * swizzle: ordering, hana::map -> hana::tuple
 * apply: f, hana::tuple -> f(*hana::tuple)
 */

#include "boost/hana.hpp"

namespace argo {

namespace bh = boost::hana;

template <typename Param, typename Value>
struct boxed_param {
  using pair        = bh::pair<Param, Value>;
  using key_type    = Param;
  using mapped_type = Value;
  using value_type  = pair;

  pair value;
};
template <typename T>
struct is_boxed_param : std::false_type {};
template <typename Param, typename Value>
struct is_boxed_param<boxed_param<Param, Value>> : std::true_type {};

template <char... Chars>
struct param : bh::string<Chars...> {
  using string = bh::string<Chars...>;
  using string::string;

  using name = string;

  template <typename T>
  auto operator=(T &&value) {
    return boxed_param<name, T>{
        bh::pair<name, T>(name{}, std::forward<T>(value))};
  }
};

namespace literals {
template <typename CharT, CharT... Chars>
constexpr auto operator"" _arg() -> param<Chars...> {
  return {};
}
} // namespace literals

template <typename... Args>
struct order {
  using names_t                  = bh::tuple<typename Args::name...>;
  static constexpr names_t names = {};
};

template <typename... Args>
constexpr auto argspec(Args... args) {
  return order<Args...>{};
}

template <typename... _>
struct undef;

template <typename... Args>
auto unpack() {
  auto const index_of = [](auto p) {
    using namespace bh::literals;
    return p[0_c];
  };
  auto const is_not_param = [](auto p) {
    using namespace bh::literals;
    return std::negation<is_boxed_param<std::decay_t<decltype(p[1_c])>>>{};
  };

  auto types = bh::tuple<Args...>{};
  auto indexes =
      bh::to_tuple(bh::range_c<std::size_t, size_t(0), sizeof...(Args)>);
  auto indexed = bh::zip(indexes, types);
  // undef<decltype(indexed)>{};
  auto args_kwargs = bh::span(indexed, is_not_param);
  // undef<decltype(args_kwargs)>{};
  auto args   = bh::transform(bh::first(args_kwargs), index_of);
  auto kwargs = bh::transform(bh::second(args_kwargs), index_of);
  return bh::make_pair(args, kwargs);
}

auto collect = [](auto argspec, auto arg_idx, auto kwarg_idx, auto... argptrs) {
  using namespace bh::literals;
  auto const all_args = bh::make_tuple(argptrs...);
  auto const get_arg  = [&all_args](auto i) { return all_args[i]; };

  auto const arg_ptrs = bh::transform(arg_idx, get_arg);
  auto const args     = bh::zip_shortest_with(
      [](auto n, auto p) { return bh::make_pair(n, p); }, argspec, arg_ptrs);

  auto const kwargs_ptrs = bh::transform(kwarg_idx, get_arg);
  auto const kwargs      = bh::transform(kwargs_ptrs, [](auto p) {
    auto &&    pv    = *p;
    auto const name  = bh::first(pv.value);
    auto const value = std::addressof(bh::second(pv.value));
    return bh::make_pair(name, value);
  });

  auto const pairs = bh::concat(args, kwargs);
  return bh::to_map(pairs);
};

auto invoke_swizzled = [](auto order, auto &&map, auto &&f) -> decltype(auto) {
  auto params =
      bh::transform(order, [&](auto const &key) { return *map[key]; });
  return bh::unpack(params, f);
};

auto transform = [](auto argspec, auto collected) { return collected; };
} // namespace argo
