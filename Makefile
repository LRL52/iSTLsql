source = main.cpp
target = main

CXX = g++
CXXFLAG = -O2 -Wall -g -std=c++23
ASAN = -fsanitize=address
LIB =

all: $(target)

main: $(source)
	$(CXX) $(source) $(CXXFLAG) $(ASAN) -o $(target) $(LIB)

clean:
	rm $(target)