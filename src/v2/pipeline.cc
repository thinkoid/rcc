// -*- mode: c++; -*-

// The pipeline of the typed tree, declared in pipeline.hh: a second
// design of the tree and of its generators, grown beside rcc's own
// and linked under the same driver as rcc-v2, so that the two compile
// one source side by side.  It takes the lexer as it is, so a lexical
// error is reported as rcc reports it; the parser builds the typed
// tree, and each target writes its text from it.  A construct rcc-v2
// does not compile yet is the parser's error, "not yet: ", at its
// token.  No warning is made, so the list is left as it is.

#include "pipeline.hh"

#include "generate.hh"
#include "lex.hh"
#include "parse.hh"

namespace rcc {

void
compile(std::string_view text,
        std::string_view file,
        const target_t &target,
        std::vector< warning_t > &,
        std::ostream &assembly)
{
    const auto tokens = tokenize(text, file);
    const auto unit = parse(tokens);

    generate(unit, target, file, assembly);
}

} // namespace rcc
