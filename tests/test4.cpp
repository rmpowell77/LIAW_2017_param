#include "argo.hpp"

#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

int foo(int a, int b, int c) { return std::pow(a, b) * c; }

constexpr inline auto const foo_args = [] {
  using namespace argo::literals;
  return argo::argspec("a"_arg, "b"_arg, "c"_arg = 5);
}();

inline auto const kwfoo = argo::adapt(foo_args, foo);

using namespace argo::literals;
TEST_CASE( "Testing basic placement arguments", "[argo1]" ) {
    REQUIRE( kwfoo(2, 3, 4) == 32 );
    REQUIRE( kwfoo(2, 3) == 40 );
    REQUIRE( kwfoo(2, 3, "c"_arg = 4) == 32 );
    REQUIRE( kwfoo(2, "b"_arg = 3) == 40 );
}

