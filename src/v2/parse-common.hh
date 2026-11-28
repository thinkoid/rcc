// -*- mode: c++; -*-

// What the parser's files share: the state of one parse, an operand,
// the diagnostics, the token primitives, the scopes, and the
// productions one file calls in another.  The parser is a recursive
// descent with one function for each production of the C grammar,
// named as rcc's parser names it (docs/typed-tree.md 4.1), and each
// file holds the productions of one clause.  A production's function
// is the accumulator of its parts, and ends in one builder call (4.4).
// Paragraph numbers are C23 (N3220).

#pragma once

#include "build.hh"
#include "expr.hh"
#include "lex.hh"
#include "stmt.hh"
#include "target.hh"
#include "type.hh"
#include "unit.hh"

#include <cstddef>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rcc {

// What the parser holds of an expression: its C type, which the
// productions read; the node built for it, or none for a function
// designator, whose value, the function's address, 6.3.2.1p4, rcc-v2
// does not compile yet; the identifier the expression is, through
// parentheses, 6.5.2p6, or null, for the diagnostics that name it and
// for the function a call names; and whether it is a constant reached
// through a signed overflow, 6.6p4, the mark every operator's
// production sets, overflowed(), and a constant expression reads,
// warn_overflowed(); whether it is the result of *, through
// parentheses, a place through a pointer, whose step and compound
// assignment are not yet; and the token of a cast of an object to a
// type of its width, narrower than an int or an unsigned, through
// parentheses and the casts that widen it, or null, for the not yet of
// its index, widened_index()'s; and the type of an array, or none: an
// expression of an array's type is converted to a pointer to its first
// element, 6.3.2.1p3, which is its C type here, read by every
// production, and the node is the array's decay, since no row reads an
// array whole, but for & and sizeof, which read the array's type, and
// so take it through parentheses.
struct operand_t
{
    type_t type;
    std::optional< expr_t > node;
    const token_t *name = nullptr;
    bool overflowed = false;
    bool indirect = false;
    const token_t *kept_bits = nullptr;
    std::optional< type_t > array = std::nullopt;
};

// What a scope holds of a name it declares, 6.2.1p4: the operand a use
// copies, its type and its leaf (docs/typed-tree.md 4.5), an
// enumeration constant's the literal of its value, 6.4.4.4p2; whether the
// name has linkage, 6.2.2: a block's extern has, and a local, a
// parameter and a block's static object have none, p6, which a block's
// extern of the name, linked_entity(), reads; and whether the name is a
// typedef name, 6.7.9p3, whose operand holds the type it names and no
// node, since a typedef declares no object.
struct symbol_t
{
    operand_t operand;
    bool linked = false;
    bool type_name = false;
};

// A scope, 6.2.1p4: the file's, a block's or a parameter list's.  It
// holds the ordinary identifiers it declares, 6.2.3p1, each a view into
// the tokens and what the scope holds of it, the file's its typedef
// names and enumeration constants alone, its functions' and its
// objects' names being parser_t's; and the tags of the enumerations it
// defines, each the enumerated type, or while its list is read the
// type of the enum in its own list, 6.7.3.4p5.
struct scope_t
{
    std::unordered_map< std::string_view, symbol_t > names = { };
    std::unordered_map< std::string_view, type_t > tags = { };
};

// What the parser holds of a function's name in the file's scope,
// 6.2.1p4: its type, a function_t, which a later declaration of the
// name must be compatible with, 6.7.1p5, and the index of its entry
// among the unit's functions, which holds its linkage and its body.
struct function_name_t
{
    type_t type;
    std::size_t index;
};

// What the parser holds of an object's name in the file's scope,
// 6.2.1p4: its type, an integer type, which a later declaration of the
// name must be compatible with, 6.7.1p5; its leaf, a place over the
// where global_t built at its first declaration, which a use copies
// (docs/typed-tree.md 4.5); whether its linkage is external, 6.2.2, set
// by its first declaration; and the index of its entry among the unit's
// objects, made by its first declaration that defines it, or none for an
// object only declared so far.
struct object_name_t
{
    type_t type;
    expr_t leaf;
    bool external;
    std::optional< std::size_t > index = std::nullopt;
};

// What the parser holds of a C label's name in the function being
// defined, 6.2.1p3: its number among the function's C labels, given at
// the first meeting of the name, a goto or its definition, in the order
// they are met (docs/typed-tree.md 4.6), and whether a label of the
// name is defined, which a name is once, 6.8.2p3.
struct label_name_t
{
    int number;
    bool defined = false;
};

// What the parser holds of a switch whose body is being read, rcc's
// open_switch_t: what it knows of the switch, its number among the
// function's loops and switches and whether a break of it stands in its
// body; the promoted type of its controlling expression, which each
// case's constant is converted to, 6.8.5.3p5; its cases in the order
// they are written; the token of each value's first case, by the value
// after the conversion, for the error of a second, 6.8.5.3p3; and the
// number of its default label and the label's token, or none.
struct open_switch_t
{
    jump_target_t target;
    type_t type;
    std::vector< case_t > cases = { };
    std::unordered_map< long long, const token_t * > values = { };
    std::optional< int > default_ = std::nullopt;
    const token_t *default_at = nullptr;
};

// The state of one parse: the tokens and the position in them; the
// target, whose plain char the parser makes; the warnings, a list the
// driver owns; the unit, filled as its functions and objects are
// declared and defined; the names of the functions and of the objects
// the file declares, views into the tokens; each use of a static
// function, by its index among the unit's functions, for the end of the
// unit to require its definition; the result type of the function being
// defined, for its return statements; the scopes open where the parse
// stands, innermost last, the file's first, then the function's blocks'
// and a parameter list's; the count of the locals the function has
// declared, which numbers the next one; the loops and the switches
// whose bodies are being read, each innermost last, with the count of
// the function's loops and switches, which numbers the next of either,
// so the innermost of the two is the one of the larger number
// (docs/typed-tree.md 4.6), and the count of the function's case and
// default labels, which numbers the next; the function's C labels by
// their names, their count numbering the next one, with the name of
// each goto in the order they are written, for the function's end to
// require their labels; the function's name, and the count of its
// block's static objects of each name, which their labels read,
// static_label(); the objects a block declared extern first, of
// external linkage, which no declaration at file scope has declared
// yet, unit_entity()'s; and the count of the identities the parser has
// given the enumerated types and the types of an enum in its own list,
// 6.2.5p21, which identifies the next.
struct parser_t
{
    const std::vector< token_t > &tokens;
    const target_t &target;
    std::vector< warning_t > &warnings;
    std::size_t pos = 0;
    translation_unit_t unit = { };
    std::unordered_map< std::string_view, function_name_t > functions = { };
    std::unordered_map< std::string_view, object_name_t > objects = { };
    std::vector< std::pair< std::size_t, const token_t * > >
        internal_uses = { };
    type_t result = { };
    std::vector< scope_t > scopes = { };
    int locals = 0;
    std::vector< jump_target_t > loops = { };
    std::vector< open_switch_t > switches = { };
    int loop_count = 0;
    int case_labels = 0;
    std::unordered_map< std::string_view, label_name_t > labels = { };
    std::vector< const token_t * > gotos = { };
    std::string_view function = { };
    std::unordered_map< std::string_view, int > statics = { };
    std::unordered_map< std::string_view, object_name_t > block_entities = { };
    int identities = 0;
};

//
// diagnostics
//

// An error of the program at the token, the whole of its message.
[[noreturn]] void fail(const token_t &at, std::string what);

// A construct of C that rcc-v2 does not compile yet, refused at the
// token by its name, as an error is.
[[noreturn]] void not_yet(const token_t &at, std::string_view construct);

// A warning at the token, which does not stop the translation: it joins
// the driver's list, which the driver prints after the parse, or before
// the error that ends it.
void warn(parser_t &p, const token_t &at, std::string what);

std::string quoted(std::string_view name);

//
// identifiers
//

// Whether a name is reserved for any use, 6.4.2.1p7: it begins with two
// underscores, or with one and an uppercase letter.
bool is_reserved(std::string_view name);

//
// token primitives
//

// The token ahead of the position; past the end, the eof that closes
// the vector.
const token_t &peek(const parser_t &p, std::size_t ahead = 0);

bool next_is(const parser_t &p, std::initializer_list< tok_t > kinds);

// Consumes the token ahead and returns it; the eof is never consumed.
const token_t &advance(parser_t &p);

// Consumes the token ahead if it is of the kind, and says whether it
// was.
bool accept(parser_t &p, tok_t kind);

// The message is the whole diagnostic, said at the token found instead.
const token_t &expect(parser_t &p, tok_t kind, std::string_view message);

//
// types
//

// int as the parser makes it: 32 bits, signed, of the kind int.
type_t int_type();

// The integer promotions, 6.3.1.1p2, rcc's promoted(): the type an
// operand of the type t is promoted to.
type_t promoted(const type_t &t);

// The machine's view of an integer type, its width, and its sign, the
// one place the parser maps a C type to the machine's
// (docs/typed-tree.md principle 9): every builder it calls is handed
// the view of the C type it means, and a widening the sign.
view_t view_of(const type_t &t);
sign_t sign_of(const type_t &t);

// The type a pointer type points to, 6.2.5p27.
const type_t &pointee_of(const type_t &t);

// The operand of an array, 6.3.2.1p3, of the type t and of the node of
// its decay: of the type of the decay, a pointer to the first element,
// and of the array's own type, which & and sizeof read.
operand_t array_operand(const type_t &t, expr_t decay);

// A type rcc declares and does not use yet, an enum in its own list,
// refused at the token of its use by its words, rcc's
// require_honoured().
void refuse_not_yet_type(const token_t &at, const type_t &t);

//
// values
//

// A void expression has no value, 6.3.2.2p1, and its use as one is an
// error at the token, rcc's require_value().
void require_value(const token_t &at, const operand_t &e);

// An operand whose value is read, rcc's rvalue(): a void one is none,
// require_value()'s, an error at the token; a function designator's
// value is its function's address, 6.3.2.1p4, which is not yet; any
// other is its node, the lvalue converted, 6.3.2.1p2.
expr_t rvalue(const token_t &at, operand_t e);

// An operand evaluated for its effect alone, 6.8.4p2, rcc's
// discarded(): it may be void, and a function designator, which would
// decay to its function's address, is not yet.
expr_t discarded(operand_t e);

// The conversion of the value of e, an operand of a value, to the type
// t, 6.3.1.3, as a node, whatever the two types: its builder is handed
// the view and the sign of t, and those of e's own type, which it reads
// off e.  Every conversion the parser builds is this function's, and
// nothing else calls make_conversion(): so a node that is no conversion
// has the type its consumer converts it from, and the width rule, which
// a widening reads by the sign of the type it converts from, is right
// by construction (docs/typed-tree.md 4.2).  A widening of a
// conditional is asked of the node, by its builder: whether rcc hands
// it to the arms is its row's, and otherwise it extends the join where
// the width the join records is short (register-stack.hh).
expr_t conversion(const operand_t &e, const type_t &t);

// The value of e in the type t, rcc's convert_to(), the place an
// implicit conversion enters the tree: no node where e's type is t in
// the machine's view, the C type changed in place, as long to long
// long; conversion()'s otherwise, which folds a constant.
expr_t convert_to(const operand_t &e, const type_t &t);

// Whether the value of e is a pointer: e is of a pointer type, or a
// function designator, which converts to a pointer to its function,
// 6.3.2.1p4; and what it points to, the pointee or the function.
bool points(const operand_t &e);
const type_t &pointee(const operand_t &e);

// Whether e is a null pointer constant, 6.3.2.3p3, rcc's
// is_null_pointer_constant(): an integer constant expression of value
// 0, which the builders fold to a constant, its value asked through the
// model (docs/typed-tree.md 4.3).  A constant of a pointer type,
// (int *) 0, is a null pointer, and no null pointer constant.
bool is_null_pointer_constant(const operand_t &e);

// A value converted to the type t as if by assignment, 6.5.17.2p2,
// rcc's assignment_conversion(): a void value is none,
// require_value()'s; an integer converts to a pointer only as a null
// pointer constant, a pointer to no integer, and a pointer to a pointer
// to a compatible type alone, 6.5.17.2p1, a function designator being a
// pointer to its function, each an error at the token; any other value
// is converted to t, convert_to()'s.  Returns the value's node.
expr_t assignment_conversion(const token_t &at, operand_t e, const type_t &t);

//
// scopes
//

// What the innermost scope that declares the name holds of it, or
// null when none does.
const symbol_t *lookup(const parser_t &p, std::string_view name);

// Declares the name in the innermost scope as a local of the type, an
// array among them, and returns the local's number.
int declare(parser_t &p, const token_t &name, const type_t &type);

//
// the productions one file calls in another
//

// 6.4.4.2, 6.4.4.5, parse-constants.cc
operand_t integer_constant(parser_t &);
operand_t character_constant(parser_t &);

// A constant's value exactly, past 64 bits, where a sum, a difference
// or a product of two 64-bit values is judged: GNU C's __int128.
using exact_t = __int128;

// Whether an integer type holds a value: two's complement over its
// width, a signed type's top bit its sign.
bool holds(const type_t &t, exact_t v);

// The value of a constant of the integer type t exactly, from bits, the
// value constant_of() reads of its node, its register's bits read signed
// at 32 bits or more: an unsigned type's are read at its width with no
// sign.
exact_t exact_value(const type_t &t, long long bits);

// The literal of the value v of the integer type t of 32 bits or 64,
// which t holds: the bits its register holds, read signed at t's width,
// so the unsigned 4294967295 is -1 (docs/typed-tree.md 4.3).
expr_t literal_of(const type_t &t, exact_t v);

// The mark of a signed overflow, 6.6p4, of an operator of the C type t,
// the operation's, on its operands, lhs and rhs, l and r their nodes as
// the operator reads them, rcc's folded_binary() and folded_unary():
// where every operand is a constant, constant_of(), a result its type
// does not hold, or an operand the operator evaluates so marked; none
// where one is no constant, since the node is then no constant
// expression.  Every production that calls an operator's builder sets
// its operand's mark so; a cast, the unary plus and the parentheses
// hand the operand's mark on.
bool overflowed(binary_op_t op,
                const type_t &t,
                const operand_t &lhs,
                const expr_t &l,
                const operand_t &rhs,
                const expr_t &r);
bool overflowed(unary_op_t op,
                const type_t &t,
                const operand_t &arg,
                const expr_t &a);

// rcc's warn_overflowed(): an expression where a constant expression is
// required, warned of at its first token, at, where its mark is set,
// but in a system header; the wrapped value is taken, as G1 has the
// same arithmetic at run time.
void warn_overflowed(parser_t &, const token_t &at, const operand_t &e);

// 6.5, parse-expressions.cc
operand_t conditional_expression(parser_t &);
operand_t assignment_expression(parser_t &);
operand_t expression(parser_t &);

// 6.7, parse-declarations.cc

// What the parser holds of the declaration specifiers: the storage
// class, static, extern, typedef or none; the first type specifier's
// token, a typedef name's or an enum specifier's among them; the type;
// and whether the type specifier is an enum specifier, which declares
// its tag or its constants, so that a declaration of it alone, enum e {
// A };, declares something, 6.7.1p2.
struct specifiers_t
{
    const token_t *storage_class;
    const token_t &specifier;
    type_t type;
    bool tagged;
};

// A parameter as written: its first token; its name's token, or null
// for a parameter its declaration does not name, 6.7.7.1p1; and its
// type.
struct parameter_t
{
    const token_t &start;
    const token_t *name;
    type_t type;
};

// What the parser holds of a declarator: its name; the type it
// declares, the specifiers', an array of it or a function returning it,
// 6.7.7.3p3, 6.7.7.4p4; for a function, its parameters as written and
// the scope of its parameter list, 6.2.1p4, which holds their names and
// which a definition's body opens again, 6.8.3p2; and the first token of
// a variable length array's size, or null, which its declaration
// refuses in its words.
struct declarator_t
{
    const token_t &name;
    type_t type;
    std::vector< parameter_t > params = { };
    scope_t scope = { };
    const token_t *variable_size = nullptr;
};

bool is_declaration_start(const parser_t &, std::size_t ahead = 0);
specifiers_t declaration_specifiers(parser_t &);
type_t type_name(parser_t &);
declarator_t declarator(parser_t &, const type_t &base);
std::vector< stmt_t > declaration(parser_t &);
stmt_t for_declaration(parser_t &);

// Whether the declaration's storage class is the keyword's.
bool declared(const specifiers_t &spec, tok_t storage_class);

// Whether the declarator writes a parameter list, rcc's d.params: it
// declares a function of a type the specifiers do not name, since a
// list after the function type a typedef name names is an error,
// declarator_suffix()'s.
bool lists_parameters(const specifiers_t &spec, const declarator_t &d);

// A function declared through a typedef name of function type, typedef
// int F(int); F g;, a declarator of no parameter list, is not yet, in
// rcc's words at its name: the unit's entry of a function takes its
// parameters from the list.
void refuse_typedef_function(const specifiers_t &spec, const declarator_t &d);

// The typedef name the declarator of a declaration of the storage class
// typedef declares, 6.7.9, in the current scope, and what may follow the
// declarator.
void
type_definition(parser_t &p, const specifiers_t &spec, const declarator_t &d);

// An object of static storage of a pointer type or of an array type,
// whose data and address rcc-v2 does not write yet, refused at its name.
void refuse_static_pointer(const token_t &name, const type_t &t);
void refuse_static_array(const token_t &name, const type_t &t);

// The initializer after the '=' ahead of the object of static storage of
// the integer type t that the unit's objects hold at index, rcc's
// initialize(): an integer constant expression, 6.7.11p4, folded, which
// is the entry's value.
void initialize(parser_t &, const type_t &t, std::size_t index);

// 6.8, parse-statements.cc
std::vector< stmt_t > block_item_list(parser_t &);

// 6.9, parse-external.cc

// The visibility pragma on a name of external linkage, not yet, and
// the two linkages of one name in a unit, an error, 6.2.2p7, rcc's
// refuse_visibility() and require_linkage(): earlier is the linkage of
// the name's earlier declaration, whether external, and external the
// later's.
void refuse_visibility(const token_t &name, bool external);
void require_linkage(const token_t &name, bool earlier, bool external);

} // namespace rcc
