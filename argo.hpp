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

template <typename Param> struct boxed_param {
  using param = Param;
};
template <typename T> struct is_boxed_param : std::false_type {
};
template <typename Param>
struct is_boxed_param<boxed_param<Param>> : std::true_type {
};
template <char... Chars> struct param : bh::string<Chars...> {
  template <typename T> auto operator=(T&& value)
  {
    return bh::make_tuple(boxed_param<param>{}, std::forward<T>(value));
  }
};

namespace literals {
  template <typename CharT, CharT... Chars>
  constexpr auto operator"" _arg() -> param<Chars...>
  {
    return {};
  }
} // literals

template <typename... Args> struct order : bh::tuple<Args...> {
  using bh::tuple<Args...>::tuple;
};

template <typename... Args> constexpr auto argspec(Args... args)
{
  return order<Args...>{ args... };
}

template <typename _> struct undef;

template <typename... Args> auto unpack()
{
  auto is_p = [](auto p) {
    using namespace bh::literals;
    return is_boxed_param<decltype(p[0_c])>{};
  };
  auto index_of = [](auto p) {
    using namespace bh::literals;
    return p[0_c];
  };
  auto types = bh::to_tuple(bh::tuple<Args...>{});
  auto indexes
      = bh::to_tuple(bh::range_c<std::size_t, size_t(0), sizeof...(Args)>);
  auto args_kwargs = bh::span(bh::zip_shortest(indexes, types), is_p);
  auto args = bh::transform(bh::first(args_kwargs), indexes);
  auto kwargs = bh::transform(bh::second(args_kwargs), indexes);
//  return std::pair{ args, kwargs };
}

template <typename... Keys, typename ArgMap, typename F>
auto invoke_swizzled(argo::order<Keys...> const& keys, ArgMap&& map, F&& f)
    -> decltype(auto)
{
  return boost::hana::apply(
      f, keys | [&](auto const& key) { return map[key]; });
}
} // argo
