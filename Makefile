source = main.cpp
target = main

CXX = g++
CXXFLAG = -Wall -g -std=c++23 -DDEBUG
ASAN = -fsanitize=address
LIB =

all: $(source)
	$(CXX) $(source) $(CXXFLAG) $(ASAN) -o $(target) $(LIB)

clean:
	rm $(target)