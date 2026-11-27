// -*- mode: c++; -*-

// 6.9: the external declarations, of which rcc-v2 takes the declaration
// and the definition of a function, and parse(), declared in parse.hh,
// last.

#include "parse.hh"

#include "build.hh"
#include "parse-common.hh"

#include <string>
#include <utility>
#include <vector>

namespace rcc {

namespace {

// The function called at program startup is named main, 5.1.2.3.2p1,
// and is found by that name from outside the unit, which static, giving
// it internal linkage, 6.2.2p3, would forbid: the link would find no
// main.
void
refuse_static_main(const specifiers_t &spec, const declarator_t &d)
{
    if (spec.storage_class && d.name.text == "main")
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a declaration of 'main'");
}

// The earlier declaration of the name in the file's scope, met again,
// or null when there is none: the function of a compatible type,
// 6.7.1p5, which keeps its type, the composite of two compatible
// function types of int parameters being either.
const function_name_t *
earlier_declaration(const parser_t &p, const token_t &name, const type_t &t)
{
    const auto it = p.functions.find(name.text);
    if (it == p.functions.end())
        return nullptr;

    if (!compatible(it->second.type, t))
        fail(name, "conflicting types for " + quoted(name.text));

    return &it->second;
}

// Whether a function's linkage is external, 6.2.2: static gives it
// internal linkage, p3; one declared without a storage class has the
// linkage of the earlier declaration, or else external, p4, p5.  So
// static int f(void); int f(void) { ... } defines a function of
// internal linkage.
bool
has_external_linkage(const parser_t &p,
                     const specifiers_t &spec,
                     const function_name_t *earlier)
{
    if (spec.storage_class)
        return false;

    return !earlier || p.unit.functions[earlier->index].external;
}

// A declaration under a #pragma GCC visibility other than default gives
// that visibility to the symbol it names where the symbol has external
// linkage, as clang has it, and rcc writes every symbol visible, so
// such a declaration is not yet, judged at the declarator's name; a
// static function is what it is without the pragma.
void
refuse_visibility(const token_t &name, bool external)
{
    if (name.nondefault_visibility && external)
        not_yet(name, "pragma 'GCC visibility'");
}

// A name of internal and of external linkage in one unit is undefined,
// 6.2.2p7, and refused where static follows a declaration without it;
// the other order gives the later declaration the earlier one's
// internal linkage, p4.
void
require_linkage(const parser_t &p,
                const token_t &name,
                const function_name_t &earlier,
                bool external)
{
    if (p.unit.functions[earlier.index].external && !external)
        fail(name, "static declaration of " + quoted(name.text) +
                       " follows non-static declaration");
}

// Declares a function in the file's scope, or meets its earlier
// declaration, and gives it the declaration's linkage.  Its entry among
// the unit's functions is made at its first declaration, so the unit
// writes its functions in the order they are first declared.  Returns
// the index of the entry.
std::size_t
declare_function(parser_t &p, const specifiers_t &spec, const declarator_t &d)
{
    const function_name_t *earlier = earlier_declaration(p, d.name, d.type);
    const bool external = has_external_linkage(p, spec, earlier);
    refuse_visibility(d.name, external);

    if (earlier) {
        require_linkage(p, d.name, *earlier, external);
        return earlier->index;
    }

    const std::size_t index = p.unit.functions.size();
    p.unit.functions.push_back(
        { std::string(d.name.text), external, d.params.size() });
    p.functions.emplace(d.name.text, function_name_t { d.type, index });

    return index;
}

// The body, a compound statement, 6.9.2p1, which is a statement: the
// block of its items.  Its '{' is the token that made the declaration a
// definition.  The parameter type list and the body are one block,
// 6.9.2p10, 6.8.3p2, so the scope of the list, with its parameters, is
// opened again and the body's items are parsed into it directly: a
// local cannot declare a parameter's name again there.
stmt_t
function_body(parser_t &p, scope_t parameters)
{
    advance(p);

    p.scopes.push_back(std::move(parameters));
    std::vector< stmt_t > items = block_item_list(p);
    p.scopes.pop_back();
    expect(p, tok_t::rbrace, "expected '}' at end of function body");

    return make_block(std::move(items));
}

// rcc requires each parameter of a definition to be named, said at the
// first that is not, where C23 lets one go unnamed and inaccessible in
// the body, 6.9.2p10 and its footnote 193: the error is rcc's.
void
require_definition_parameters(const declarator_t &d)
{
    for (const parameter_t &param : d.params)
        if (!param.name)
            fail(param.start, "parameter name omitted");
}

// Each goto of the function names a label the function defines,
// 6.8.7.2p1, before the goto or after it, since a label's name has
// function scope, 6.2.1p3: so the check waits for the function's end,
// as rcc's require_labels() does, and the first goto in the order they
// are written whose label is not defined is the error, said at its
// name.
void
require_labels(const parser_t &p)
{
    for (const token_t *name : p.gotos)
        if (!p.labels.at(name->text).defined)
            fail(*name,
                 "label " + quoted(name->text) + " used but not defined");
}

// A definition of the function, declared first, so that its name is in
// scope in its own body, 6.2.1p7.  A function is defined once, 6.9.1p5.
// The body is parsed with the function's result type, its locals
// numbered after the parameters, its loops and its C labels from 0, and
// is the entry's once its gotos name labels it defines.  A system
// header's definition is compiled as any other, since rcc's rule for
// one is a static inline function's.
void
function_definition(parser_t &p, const specifiers_t &spec, declarator_t &d)
{
    const std::size_t index = declare_function(p, spec, d);
    if (p.unit.functions[index].body)
        fail(d.name, "redefinition of " + quoted(d.name.text));

    require_definition_parameters(d);

    p.result = *as< function_t >(d.type).result;
    p.locals = static_cast< int >(d.params.size());
    p.loop_count = 0;
    p.labels.clear();
    p.gotos.clear();

    stmt_t body = function_body(p, std::move(d.scope));
    require_labels(p);

    p.unit.functions[index].body = std::move(body);
}

// One declarator of a declaration in the file's scope: a function's, or
// an object's, not yet.  rcc reads GNU C's attributes and an assembler
// name after a function's declarator before it declares the function,
// and they are not yet.
void
external_declarator(parser_t &p,
                    const specifiers_t &spec,
                    const declarator_t &d)
{
    if (!is< function_t >(d.type))
        not_yet(d.name, "object " + quoted(d.name.text));

    refuse_static_main(spec, d);

    if (next_is(p, { tok_t::kw_attribute, tok_t::kw_asm }))
        not_yet(peek(p), quoted(peek(p).text) + " after a declarator");

    declare_function(p, spec, d);
}

// A function definition, or a declaration of functions, 6.9.1p1; an
// object among them is not yet.  rcc reads __asm__, __extension__ and a
// static assertion before the declaration specifiers, and refuses the
// first in these words.  A function's declarator that a '{' follows is
// a definition, and a definition is one declarator and its body; the
// declarators of a declaration are separated by commas and end at its
// ';'.
void
external_declaration(parser_t &p)
{
    const token_t &t = peek(p);
    if (t.kind == tok_t::kw_asm)
        not_yet(t, quoted(t.text) + " at file scope");

    if (t.kind == tok_t::kw_extension)
        not_yet(t, quoted(t.text) + " before a declaration");

    if (t.kind == tok_t::kw_static_assert)
        not_yet(t, quoted(t.text) + " declaration");

    const specifiers_t spec = declaration_specifiers(p);
    declarator_t d = declarator(p, spec.type);

    if (is< function_t >(d.type) && peek(p).kind == tok_t::lbrace) {
        refuse_static_main(spec, d);
        function_definition(p, spec, d);
        return;
    }

    external_declarator(p, spec, d);

    while (accept(p, tok_t::comma))
        external_declarator(p, spec, declarator(p, spec.type));

    expect(p, tok_t::semicolon, "expected ';' after top level declarator");
}

// A static function used in an expression is defined in the unit,
// 6.9.1p3: one that is not is an error at the end of the unit, said at
// its first use.
void
require_internal_definitions(const parser_t &p)
{
    for (const auto &[index, at] : p.internal_uses) {
        const function_decl_t &f = p.unit.functions[index];
        if (!f.body)
            fail(*at, "static function " + quoted(f.name) +
                          " is used but never defined");
    }
}

// The external declarations up to the end of the text, 6.9.1p1.  An
// empty text, which the grammar does not have, is a unit of no
// function, as rcc takes it.
void
translation_unit(parser_t &p)
{
    while (peek(p).kind != tok_t::eof)
        external_declaration(p);

    require_internal_definitions(p);
}

} // namespace

translation_unit_t
parse(const std::vector< token_t > &tokens,
      const target_t &target,
      std::vector< warning_t > &warnings)
{
    parser_t p { tokens, target, warnings };
    translation_unit(p);

    return std::move(p.unit);
}

} // namespace rcc
