target = main gen
CXX = g++
CXXFLAG = -O2 -Wall -g -std=c++23
ASAN = -fsanitize=address
RM = rm -f
# LIB =

all: $(target)

$(target): %: %.cpp
	$(CXX) $^ $(CXXFLAG) $(ASAN) -o $@

# main: $(source)
# 	$(CXX) $(source) $(CXXFLAG) $(ASAN) -o $(target) $(LIB)

.PHONY: clean
clean:
	$(RM) $(target)