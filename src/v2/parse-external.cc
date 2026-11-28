// -*- mode: c++; -*-

// 6.9: the external declarations, of which rcc-v2 takes the declaration
// and the definition of a function, the declaration of objects of the
// integer types and the type definitions, and parse(), declared in
// parse.hh, last.

#include "parse.hh"

#include "build.hh"
#include "parse-common.hh"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rcc {

// A declaration under a #pragma GCC visibility other than default gives
// that visibility to the symbol it names where the symbol has external
// linkage, as clang has it, a function and an object alike, and rcc
// writes every symbol visible, so such a declaration is not yet, judged
// at the declarator's name; one of a name of internal linkage is what
// it is without the pragma.
void
refuse_visibility(const token_t &name, bool external)
{
    if (name.nondefault_visibility && external)
        not_yet(name, "pragma 'GCC visibility'");
}

// A name of internal and of external linkage in one unit is undefined,
// 6.2.2p7, and refused, rcc's require_linkage(): int n; static int n;
// and static int n; int n;, whose second declaration gives the name the
// other linkage than the earlier's, earlier.
void
require_linkage(const token_t &name, bool earlier, bool external)
{
    if (earlier == external)
        return;

    if (!external)
        fail(name, "static declaration of " + quoted(name.text) +
                       " follows non-static declaration");

    fail(name, "non-static declaration of " + quoted(name.text) +
                   " follows static declaration");
}

namespace {

// The function called at program startup is named main, 5.1.2.3.2p1,
// and is found by that name from outside the unit, which static, giving
// it internal linkage, 6.2.2p3, would forbid: the link would find no
// main.  An object named main is not the function and is let be.
void
refuse_static_main(const specifiers_t &spec, const declarator_t &d)
{
    if (declared(spec, tok_t::kw_static) && d.name.text == "main" &&
        is< function_t >(d.type))
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a declaration of 'main'");
}

// The earlier declaration of the name among names, the file's functions
// or its objects, met again, or null when there is none, rcc's
// earlier_declaration() and redeclared(): a function, an object and a
// typedef name are three kinds of ordinary identifier, so a name of
// others, the other kind of entity, or of the file's scope, a typedef
// name, declared again is a redefinition, and two declarations of one
// kind are of compatible types, 6.7.1p5, or a conflict, each said at the
// later name.  The earlier declaration keeps its type, the composite of two
// compatible types being either: two integer types are compatible only
// where they are of one kind, rcc's related(), and so one type, and two
// function types where their results and their parameters are.  Two
// basic types of one representation are distinct types, 6.2.5p18: char,
// signed char and unsigned char are three, p20, and long and long long
// two, though the machine takes the two of one width and sign for one
// type.
template< typename N, typename O >
N *
earlier_declaration(std::unordered_map< std::string_view, N > &names,
                    const std::unordered_map< std::string_view, O > &others,
                    const scope_t &file,
                    const token_t &name,
                    const type_t &t)
{
    if (others.contains(name.text) || file.contains(name.text))
        fail(name, "redefinition of " + quoted(name.text));

    const auto it = names.find(name.text);
    if (it == names.end())
        return nullptr;

    if (!compatible(it->second.type, t))
        fail(name, "conflicting types for " + quoted(name.text));

    return &it->second;
}

// Whether the linkage a declaration at file scope gives its name is
// external, 6.2.2, rcc's linkage_of(): static gives internal linkage,
// p3; extern the linkage of the earlier declaration, p4, earlier, or
// else external, and a function declared without a storage class is as
// if declared extern, p5; an object declared without one has external
// linkage, p5.  So static int f(void); int f(void) { ... } defines a
// function of internal linkage, and static int n; extern int n;
// declares one object of internal linkage.
bool
linkage_of(const specifiers_t &spec,
           std::optional< bool > earlier,
           bool function)
{
    if (declared(spec, tok_t::kw_static))
        return false;

    const bool as_extern = function || declared(spec, tok_t::kw_extern);
    if (earlier && as_extern)
        return *earlier;

    return true;
}

// An object a block declared extern first, with external linkage, is
// the entity a declaration of its name at file scope denotes, 6.2.2p2,
// rcc's adopt_block_entity(): the name joins the file's objects as that
// entity, where earlier_declaration() meets it, unless the file declares
// the name already.  So int g = 1; after a block's extern int g; defines
// the block's object, static int g; is the error of its two linkages,
// and a function of the name a redefinition.
void
adopt_block_entity(parser_t &p, std::string_view name)
{
    const auto it = p.block_entities.find(name);
    if (it == p.block_entities.end() || p.objects.contains(name) ||
        p.functions.contains(name))
        return;

    p.objects.emplace(it->first, std::move(it->second));
    p.block_entities.erase(it);
}

// Declares a function in the file's scope, or meets its earlier
// declaration, and gives it the declaration's linkage.  Its entry among
// the unit's functions is made at its first declaration, so the unit
// writes its functions in the order they are first declared, with the
// view of each parameter, what its frame needs.  Returns the index of
// the entry.
std::size_t
declare_function(parser_t &p, const specifiers_t &spec, const declarator_t &d)
{
    adopt_block_entity(p, d.name.text);

    const function_name_t *earlier = earlier_declaration(
        p.functions, p.objects, p.scopes.front(), d.name, d.type);
    std::optional< bool > linked;
    if (earlier)
        linked = p.unit.functions[earlier->index].external;

    const bool external = linkage_of(spec, linked, true);
    refuse_visibility(d.name, external);

    if (earlier) {
        require_linkage(d.name, *linked, external);
        return earlier->index;
    }

    std::vector< view_t > parameters;
    for (const parameter_t &param : d.params)
        parameters.push_back(view_of(param.type));

    const std::size_t index = p.unit.functions.size();
    p.unit.functions.push_back(
        { std::string(d.name.text), external, std::move(parameters) });
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
// numbered after the parameters, its loops and switches, its case and
// default labels and its C labels from 0, and is the entry's once its
// gotos name labels it defines.  A system header's definition is
// compiled as any other, since rcc's rule for one is a static inline
// function's.
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
    p.case_labels = 0;
    p.labels.clear();
    p.gotos.clear();
    p.function = d.name.text;
    p.statics.clear();

    stmt_t body = function_body(p, std::move(d.scope));
    require_labels(p);

    p.unit.functions[index].body = std::move(body);
}

// An object at file scope, 6.9.3, rcc's declare_external_object().  A
// declaration with an initializer defines it, p1, extern or not; one
// without an initializer and without extern is a tentative definition,
// p2; extern alone, neither of the two, declares an object defined
// elsewhere, which the unit writes nothing for.  Every declaration of
// the name in the unit is of one object, of one linkage, linkage_of()'s,
// and of one type, 6.2.7p2: extern int n; int n; int n = 1; declare one
// object of external linkage, static int n; extern int n; one of
// internal linkage, which a tentative definition defines, and two
// tentative definitions define one.  The object's entry among the
// unit's objects is made by its first declaration that defines it, so
// the unit writes its objects in that order.  An object only
// tentatively defined is zero, 6.9.3p2.  A second initializer is a
// second external definition, of which a program has one, 6.9.1p5, an
// error.  The name is in scope from the end of its declarator, 6.2.1p7,
// so an initializer sees the object.  The name's leaf, a place of the
// object's view and sign over its global_t, is built by its first
// declaration, whether a later one defines the object or none does,
// since the unit's answer is generation's.  An object of type void, which
// rcc refuses where the unit defines it and lets be where it does not,
// is not yet.
void
declare_object(parser_t &p, const specifiers_t &spec, const declarator_t &d)
{
    if (is< void_t >(d.type))
        not_yet(d.name, "object " + quoted(d.name.text) + " of type 'void'");

    refuse_static_pointer(d.name, d.type);

    adopt_block_entity(p, d.name.text);

    object_name_t *object = earlier_declaration(
        p.objects, p.functions, p.scopes.front(), d.name, d.type);
    std::optional< bool > linked;
    if (object)
        linked = object->external;

    const bool external = linkage_of(spec, linked, false);
    refuse_visibility(d.name, external);

    if (!object) {
        expr_t leaf =
            make_global(d.name.text, view_of(d.type), sign_of(d.type));
        object_name_t name { d.type, std::move(leaf), external };
        object = &p.objects.emplace(d.name.text, std::move(name)).first->second;
    }

    require_linkage(d.name, object->external, external);

    const bool initialized = peek(p).kind == tok_t::assign;
    if (!initialized && declared(spec, tok_t::kw_extern))
        return;

    std::optional< std::size_t > &index = object->index;
    if (index && initialized && p.unit.objects[*index].value)
        fail(d.name, "redefinition of " + quoted(d.name.text));

    if (!index) {
        index = p.unit.objects.size();
        p.unit.objects.push_back({ std::string(d.name.text), external,
                                   view_of(d.type), sign_of(d.type) });
    }

    if (initialized)
        initialize(p, d.type, *index);
}

// One declarator of a declaration in the file's scope: a typedef
// name's, a function's or an object's.  A function declared through a
// typedef name is refused first, as rcc refuses it.  rcc reads GNU C's
// attributes and an assembler name after a declarator before it declares
// its name, and they are not yet.
void
external_declarator(parser_t &p,
                    const specifiers_t &spec,
                    const declarator_t &d)
{
    if (declared(spec, tok_t::kw_typedef)) {
        type_definition(p, spec, d);
        return;
    }

    refuse_typedef_function(spec, d);
    refuse_static_main(spec, d);

    if (next_is(p, { tok_t::kw_attribute, tok_t::kw_asm }))
        not_yet(peek(p), quoted(peek(p).text) + " after a declarator");

    if (is< function_t >(d.type))
        declare_function(p, spec, d);
    else
        declare_object(p, spec, d);
}

// A function definition, or a declaration of functions, of objects and
// of typedef names, 6.9.1p1.  extern on a function changes nothing: a
// function declared without a storage class has the linkage extern
// gives it, 6.2.2p5, and static gives it internal linkage, p3, on a
// declaration and on a definition alike.  rcc reads __asm__,
// __extension__ and a static assertion before the declaration
// specifiers, and refuses the first in these words.  A declarator that
// writes a parameter list and that a '{' follows is a definition, but
// for a typedef's, and a definition is one declarator and its body; the
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

    if (lists_parameters(spec, d) && !declared(spec, tok_t::kw_typedef) &&
        peek(p).kind == tok_t::lbrace) {
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
    p.scopes.emplace_back();
    translation_unit(p);

    return std::move(p.unit);
}

} // namespace rcc
