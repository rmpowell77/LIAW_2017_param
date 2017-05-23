#include <utility>
#include <variant>

template <typename T>
struct visit {};

template <typename... Alts>
struct visit<std::variant<Alts...>> {
  template <typename Tag, typename... Ts>
  constexpr auto operator()(Tag, Ts&&... xs) {
    return std::visit(std::forward<Ts>(xs)...);
  }
};

template <typename Trait>
struct dispatched {
  template <typename... Ts>
  constexpr operator()(Ts&&... teeng) const {
    return Trait{}(std::forward<Ts>(teeng)...);
  }
};

template <typename T>
inline constexpr dispatched<visit<T>> visit_v{};
