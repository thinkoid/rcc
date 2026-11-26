// -*- mode: c++; -*-

// The pipeline of the typed tree, declared in pipeline.hh: a second
// design of the tree and of its generators, grown beside rcc's own
// and linked under the same driver as rcc-v2, so that the two compile
// one source side by side.  It takes the lexer as it is, so a lexical
// error is reported as rcc reports it.  It compiles nothing yet: a
// source that tokenizes stops as a construct not yet compiled, the
// driver's exit status 3.

#include "pipeline.hh"

#include "lex.hh"

#include <stdexcept>

namespace rcc {

void
compile(std::string_view text,
        std::string_view file,
        const target_t &,
        std::vector< warning_t > &,
        std::ostream &)
{
    tokenize(text, file);

    throw std::logic_error("v2: not yet: a translation unit");
}

} // namespace rcc
