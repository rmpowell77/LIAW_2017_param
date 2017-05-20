
all: test
test: test.cpp argo.hpp
	clang++ "$<" -o "$@" \
		-std=c++1z \
		-I"/usr/local/include/" \
		-Wno-gnu-string-literal-operator-template \

