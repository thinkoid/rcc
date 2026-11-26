# rcc: build, test, test-long, bench, slots, sides, clean.  GNU make;
# the dependency files, the pattern rules and the target-specific
# variable are GNU.  The build is two compilers of one driver: rcc,
# and rcc-v2, the pipeline of src/v2 in place of rcc's.

CXX      = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -g -MMD -MP

SRC = $(wildcard src/*.cc)
OBJ = $(SRC:src/%.cc=build/%.o)

# rcc-v2: the driver, the lexer and the C types, then every source of
# src/v2
V2_SRC = $(wildcard src/v2/*.cc)
V2_OBJ = build/main.o build/lex.o build/type.o \
         $(V2_SRC:src/v2/%.cc=build/v2/%.o)

# the targets with pins: each directory under tests/corpus
TARGETS = $(patsubst tests/corpus/%/,%,$(wildcard tests/corpus/*/))

all: rcc rcc-v2

rcc: $(OBJ)
	$(CXX) -o $@ $^

rcc-v2: $(V2_OBJ)
	$(CXX) -o $@ $^

build/%.o: src/%.cc | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# a source of src/v2 includes what it shares with rcc by its name
build/v2/%.o: src/v2/%.cc | build/v2
	$(CXX) $(CXXFLAGS) -Isrc -c -o $@ $<

build build/v2:
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

# rcc-v2 against rcc, side by side, on every program of tests/ and
# every target; make test is rcc's alone
sides: rcc rcc-v2
	tests/sides

clean:
	rm -rf build rcc rcc-v2

-include $(OBJ:.o=.d) $(V2_OBJ:.o=.d)
.PHONY: all test $(TARGETS:%=test-%) test-long bench slots sides clean
