
all: test
test: test.cpp argo.hpp
	gcc-7 "$<" -o "$@" -std=c++17 -I"/usr/local/include/"
