// -*- mode: c++; -*-

// The one unit that sees the whole zoo, docs/typed-tree.md 3.1: the
// node types, every row of both targets and the models, so that every
// model is instantiated here against the same rows.  It defines the
// handles' own rows, which call through the interface, and the
// builders, declared in build.hh, which make the models.

#include "build.hh"

#include "amd64.hh"
#include "arm64.hh"
#include "model.hh"
#include "rows-statements.hh"
#include "zoo.hh"

#include <utility>
#include <vector>

namespace rcc {

void
into(amd64::state_t &s, const expr_t &e, amd64::reg_t r)
{
    e.self->into(s, r);
}

void
into(arm64::state_t &s, const expr_t &e, arm64::reg_t r)
{
    e.self->into(s, r);
}

void
generate(amd64::state_t &s, const stmt_t &statement)
{
    statement.self->generate(s);
}

void
generate(arm64::state_t &s, const stmt_t &statement)
{
    statement.self->generate(s);
}

expr_t
make_literal(int value)
{
    return expr_t(literal_t { value });
}

stmt_t
make_return(expr_t value)
{
    return stmt_t(return_t { std::move(value) });
}

stmt_t
make_block(std::vector< stmt_t > items)
{
    return stmt_t(block_t { std::move(items) });
}

} // namespace rcc
