#include <type_traits>
#include <utility>

template<class Spelling> struct KW;
template<class Spelling, class ValueType> struct KW_with_value;

template<class Spelling>
struct KW {
    template<class ValueType>
    auto operator<< (ValueType&& v) const {
        return KW_with_value<Spelling, ValueType>(std::forward<ValueType>(v));
    }
};

template<class Spelling, class ValueType>
struct KW_with_value : KW<Spelling> {
    ValueType&& v;
    template<class V, class=std::enable_if_t<std::is_same_v<std::decay_t<V>,std::decay_t<ValueType>>>>
    constexpr explicit KW_with_value(V&& v) : v(std::forward<V>(v)) {}
    decltype(auto) forward_value() { return std::forward<ValueType>(v); }
};

template<class KW_FOO, class Arg, class... Args>
decltype(auto) find_arg(KW_FOO kw, Arg&& arg, Args&&... args) {
    if constexpr (std::is_base_of<KW_FOO, std::decay_t<Arg>>::value) {
        return std::forward<Arg>(arg);
    } else {
        return find_arg(kw, std::forward<Args>(args)...);
    }
}

template<class... KEYWORD>
struct keyword_augmented {
    template<class Func, class... Args>
    decltype(auto) operator()(Func&& real_function, Args&&... args) const {
        return std::forward<Func>(real_function)(
            find_arg(KEYWORD{}, std::forward<Args>(args)...).forward_value()
            ...
        );
    }
};


void real_foo(int& a, int b) { a += b; }

using KW_A = KW<std::integral_constant<char, 'A'>>;
using KW_B = KW<std::integral_constant<char, 'B'>>;


template<class... Args>
auto foo(Args&&... args)
{
    return keyword_augmented<KW_A, KW_B>{}(real_foo, std::forward<Args>(args)...);
}

int main()
{
    int i=42;
    foo(KW_B{} << 42, KW_A{} << i, KW_B{} << 41);
    return i;
}
