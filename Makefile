source = main.cpp
target = main

CXX = g++
CXXFLAG = -Wall -g -std=c++23 
ASAN = -fsanitize=address
LIB =

all:
	$(CXX) $(source) $(CXXFLAG) $(ASAN) -o $(target) $(LIB)

clean:
	rm $(target)