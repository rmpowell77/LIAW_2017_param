
kwargs ::hana::map<param, {nothing, default<T>, value<T>}>;

make_argspec: ordering, defaults, reflow spec -> argspec

unpack :: hana::tuple -> hana::tuple, hana::map
unpack: arglist -> args, kwargs

collect ::  -> hana::map
collect: args, kwargs, ordering -> kwargs
  for x, v in zip(ordering, args):
      assert kwargs.insert(x, v).second

transform: argspec, hana::map -> hana::map (fuller)
swizzle: ordering, hana::map -> hana::tuple
apply: f, hana::tuple -> f(*hana::tuple)



auto const argspec = reflow::graph(
    "width"_p = {
      "type"_p = hana::is_numeric,
      "productions"_p = { { "height"_p, "aspect_ratio"_p },
                          [](auto height, auto aspect_ratio) {
    return; } },
      "checker"_p = [](auto x, reflow::map const& p) {
    return x > 0; },
    "height"_p = {
      [](auto x, reflow::map const& p) {
    return true; },
    "aspect_ratio"_p = {
      [](auto x, reflow::map const& p) {
    return
      }
    }
    );
template <typename... Args,
         std::enable_if<argspec::check(std::declval<Args>()...), nullptr_t> =
             nullptr>
void foo(Args&&... args) -> decltype(enable_if<argspec::check(args...)>, foo)) {
  auto const args         = reflow::parse(argspec, std::forward<Args>(args)...);
  auto       my_int       = args["foo"_p];
  auto       my_something = args["bar"_p];
  /*...*/
}
