
CXXFLAGS= \
		-std=c++1z \
		-I"/usr/local/include/" \
		-Wall -Wextra \
		-Wno-gnu-string-literal-operator-template \
		-O3 \


all: test_main test.s test_main.s test_calls.s

test.s: test.cpp test.hpp argo.hpp
	clang++ "$<" -S -o "$@" \
		$(CXXFLAGS)

test_calls.s: test_calls.cpp test.hpp argo.hpp
	clang++ "$<" -S -o "$@" \
		$(CXXFLAGS)

test_main.s: test_main.cpp test.hpp argo.hpp
	clang++ "$<" -S -o "$@" \
		$(CXXFLAGS)

test.o: test.cpp test.hpp argo.hpp
	clang++ "$<" -c -o "$@" \
		$(CXXFLAGS)

test_main.o: test_main.cpp
	clang++ "$<" -c -o "$@" \
		$(CXXFLAGS)

test_calls.o: test_calls.cpp
	clang++ "$<" -c -o "$@" \
		$(CXXFLAGS)

test_main: test_main.o test.o test_calls.o
	clang++ $^ -o "$@" \
		$(CXXFLAGS)
