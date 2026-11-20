# rcc: build, test, test-long, bench, slots, clean.  GNU make; the
# dependency files, the pattern rules and the target-specific
# variable are GNU.

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

# the oracle's options: none for make test, which leaves the programs
# of the long rows unrun
ORACLE_FLAGS =

# the pins of every target, then the driver's command line, then the
# probes, which run what rcc writes and so only for this machine's own
# target
test: $(TARGETS:%=test-%) rcc
	tests/driver
	tests/probes

$(TARGETS:%=test-%): test-%: rcc
	tests/oracle --target $* $(ORACLE_FLAGS)
	tests/costs --target $*
	tests/errors --target $*

# make test, with the oracle's long rows run on this machine's own
# target, each to the time limit; another target's runs nothing
test-long: ORACLE_FLAGS = --long
test-long: test

bench: rcc
	tests/bench

# the slot's cost; CORE=<n> pins the runs to core <n>
slots: rcc
	tests/slots $(if $(CORE),--core $(CORE))

clean:
	rm -rf build rcc

-include $(OBJ:.o=.d)
.PHONY: test $(TARGETS:%=test-%) test-long bench slots clean
