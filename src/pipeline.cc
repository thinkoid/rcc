// -*- mode: c++; -*-

// rcc's pipeline, declared in pipeline.hh: the lexer, the parser,
// which makes a typed unit, and c1, which writes the unit's assembly.

#include "pipeline.hh"

#include "c1.hh"
#include "lex.hh"
#include "parse.hh"

namespace rcc {

void
compile(std::string_view text,
        std::string_view file,
        const target_t &target,
        std::vector< warning_t > &warnings,
        std::ostream &assembly)
{
    const auto tokens = tokenize(text, file);
    const auto unit = parse(tokens, target, warnings);

    generate(unit, target, file, assembly);
}

} // namespace rcc
