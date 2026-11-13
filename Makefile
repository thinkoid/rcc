# rcc: build, test, bench, slots, clean.  GNU make; the dependency
# files and the pattern rules are GNU.

CXX      = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -g -MMD -MP

SRC = $(wildcard src/*.cc)
OBJ = $(SRC:src/%.cc=build/%.o)

# the targets with pins: each directory under tests/corpus
TARGETS = $(patsubst tests/corpus/%/,%,$(wildcard tests/corpus/*/))

rcc: $(OBJ)
	$(CXX) -o $@ $^

build/%.o: src/%.cc | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build:
	mkdir -p $@

# the pins of every target, then the probes, which run what rcc writes
# and so only for this machine's own target
test: $(TARGETS:%=test-%) rcc
	tests/probes

$(TARGETS:%=test-%): test-%: rcc
	tests/oracle --target $*
	tests/costs --target $*

bench: rcc
	tests/bench

# the slot's cost; CORE=<n> pins the runs to core <n>
slots: rcc
	tests/slots $(if $(CORE),--core $(CORE))

clean:
	rm -rf build rcc

-include $(OBJ:.o=.d)
.PHONY: test $(TARGETS:%=test-%) bench slots clean
