#include "test.hpp"

using namespace argo::literals;
int call_kwfoo() { return kwfoo(2, "c"_arg = 3, "b"_arg = 4); }

int call_kwfoo2() { return kwfoo2(2, "c"_arg = 3, "b"_arg = 4); }
