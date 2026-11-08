# rcc: build, test, bench, clean.  GNU make; the dependency files and
# the pattern rules are GNU.

CXX      = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -g -MMD -MP

SRC = $(wildcard src/*.cc)
OBJ = $(SRC:src/%.cc=build/%.o)

rcc: $(OBJ)
	$(CXX) -o $@ $^

build/%.o: src/%.cc | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build:
	mkdir -p $@

test: rcc
	tests/oracle
	tests/costs

test-native: rcc
	tests/oracle --native
	tests/costs --native

bench: rcc
	tests/bench

clean:
	rm -rf build rcc

-include $(OBJ:.o=.d)
.PHONY: test test-native bench clean
