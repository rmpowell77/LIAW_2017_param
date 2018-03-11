#include "argo.hpp"

#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

auto foo(std::string a, int b) { return a + std::to_string(b); }

constexpr inline auto const foo_args = [] {
  using namespace argo::literals;
  return argo::argspec("a"_arg, "b"_arg);
}();

inline auto const kwfoo = argo::adapt(foo_args, foo);

using namespace argo::literals;
TEST_CASE( "Testing conversions", "[argo2]" ) {
    REQUIRE( kwfoo("x = ", 3) == "x = 3" );
    REQUIRE( kwfoo("x = ", "b"_arg = 3) == "x = 3" );
    REQUIRE( kwfoo("a"_arg = "x = ", "b"_arg = 3) == "x = 3" );
    REQUIRE( kwfoo("a"_arg = std::string("x = "), "b"_arg = 3) == "x = 3" );
    REQUIRE( kwfoo("b"_arg = 3, "a"_arg = std::string("x = ")) == "x = 3" );
}

TEST_CASE( "Testing lvalues", "[argo2]" ) {
    int b = 3;
    float b_f = 3.3;
    std::string a = "x = ";
    REQUIRE( kwfoo("b"_arg = b, "a"_arg = a) == "x = 3" );
    REQUIRE( kwfoo("b"_arg = b_f, "a"_arg = a) == "x = 3" );
}

std::string get_string() { return "x = "; }
auto get_value() { return 3; }

TEST_CASE( "Testing rvalues", "[argo2]" ) {
    REQUIRE( kwfoo("b"_arg = get_value(), "a"_arg = get_string()) == "x = 3" );
}

TEST_CASE( "Testing changing values", "[argo2]" ) {
    int b = 3;
    std::string a = "x = ";
    REQUIRE( kwfoo("b"_arg = b, "a"_arg = a) == "x = 3" );
    a = "y = ";
    REQUIRE( kwfoo("b"_arg = b, "a"_arg = a) == "y = 3" );
}

