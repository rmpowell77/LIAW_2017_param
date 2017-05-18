#include <tuple>
#include <type_traits>
#include <utility>

template<size_t I> using index_constant = std::integral_constant<size_t, I>;

template<class KW> struct keyword_only {};
template<class KW> struct positional_only {};
template<class Spelling> class KW;
template<class Base, class ValueType> class KW_with_value;

template<class Spelling>
class KW {
public:
    template<class ValueType>
    auto operator<< (ValueType&& v) const {
        return KW_with_value<KW, ValueType>(std::forward<ValueType>(v));
    }
};

template<class Base, class ValueType>
class KW_with_value : public Base {
    ValueType&& v;
public:
    template<class V, class=std::enable_if_t<std::is_same_v<std::decay_t<V>,std::decay_t<ValueType>>>>
    constexpr explicit KW_with_value(V&& v) : v(std::forward<V>(v)) {}
    decltype(auto) forward_value() { return std::forward<ValueType>(v); }
};

template<class KW_FOO, class Arg, class... Rest>
decltype(auto) find_arg_recursive(KW_FOO kw, Arg&& arg, Rest&&... rest) {
    if constexpr (std::is_base_of_v<KW_FOO, std::decay_t<Arg>>) {
        return std::forward<Arg>(arg).forward_value();
    } else {
        return find_arg_recursive(kw, std::forward<Rest>(rest)...);
    }
}

template<class KW_FOO, size_t Pos, class... Args>
decltype(auto) find_arg(KW_FOO kw, index_constant<Pos>, Args&&... args) {
    if constexpr ((... + std::is_base_of_v<KW_FOO, std::decay_t<Args>>) == 0) {
        return std::get<Pos>(std::forward_as_tuple(std::forward<Args>(args)...));
    } else {
        static_assert((... + std::is_base_of_v<KW_FOO, std::decay_t<Args>>) <= 1, "caller provided a duplicate keyword argument");
        static_assert((... + std::is_base_of_v<KW_FOO, std::decay_t<Args>>) >= 1, "caller provided no value for a required argument");
        return find_arg_recursive(kw, std::forward<Args>(args)...);
    }
}

template<class KW_FOO, size_t Pos, class... Args>
decltype(auto) find_arg(keyword_only<KW_FOO> kw, index_constant<Pos>, Args&&... args) {
    static_assert((... + std::is_base_of_v<KW_FOO, std::decay_t<Args>>) <= 1, "caller provided a duplicate keyword argument");
    static_assert((... + std::is_base_of_v<KW_FOO, std::decay_t<Args>>) >= 1, "caller provided no value for a required argument");
    return find_arg_recursive(kw, std::forward<Args>(args)...);
}

template<class KW_FOO, size_t Pos, class... Args>
decltype(auto) find_arg(positional_only<KW_FOO>, index_constant<Pos>, Args&&... args) {
    static_assert((... + std::is_base_of_v<KW_FOO, std::decay_t<Args>>) == 0, "caller attempted to provide a keyword value for a positional-only argument");
    return std::get<Pos>(std::forward_as_tuple(std::forward<Args>(args)...));
}

template<class... KEYWORD>
class keyword_augmented {
public:
    template<class Func, class... Args>
    decltype(auto) operator()(Func&& real_function, Args&&... args) const {
        return call(std::index_sequence_for<Args...>{}, std::forward<Func>(real_function), std::forward<Args>(args)...);
    }

    template<size_t... Is, class Func, class... Args>
    decltype(auto) call(std::index_sequence<Is...>, Func&& real_function, Args&&... args) const {
        return std::forward<Func>(real_function)(
            find_arg(KEYWORD{}, index_constant<Is>{}, std::forward<Args>(args)...)
            ...
        );
    }
};


#include <stdio.h>

void real_foo(int& a, int b) { printf("%d %d\n", a, b); }

using KW_A = KW<std::integral_constant<char, 'A'>>;
using KW_B = KW<std::integral_constant<char, 'B'>>;

template<class... Args>
auto foo(Args&&... args)
{
    return keyword_augmented<
        positional_only<KW_A>,
        KW_B
    >{}(real_foo, std::forward<Args>(args)...);
}

int main()
{
    int a = 1;
    int b = 2;
    foo(a, b);
    foo(a, KW_B{} << b);
}
