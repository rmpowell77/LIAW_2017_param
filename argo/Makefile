
CXXFLAGS= \
		-std=c++1z \
		-I"/opt/local/include/" \
		-Wall -Wextra \
		-Wno-gnu-string-literal-operator-template \
		-O3 \


all: test_main foo.s test_main.s

foo.s: foo.cpp foo.hpp
	clang++ "$<" -S -o "$@" \
		$(CXXFLAGS)

test_main.s: test_main.cpp foo.hpp argo.hpp
	clang++ "$<" -S -o "$@" \
		$(CXXFLAGS)

foo.o: foo.cpp foo.hpp
	clang++ "$<" -c -o "$@" \
		$(CXXFLAGS)

test_main.o: test_main.cpp
	clang++ "$<" -c -o "$@" \
		$(CXXFLAGS)

test_main: test_main.o foo.o
	clang++ $^ -o "$@" \
		$(CXXFLAGS)
