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
#include <deque>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rcc {

struct record_pointer_t;

// What the parser holds of an expression: its C type, which the
// productions read; the node built for it, or none for a function
// designator, whose value, the function's address, 6.3.2.1p4, rcc-v2
// does not compile yet, and for a parameter of a record type in a
// declaration, which no body reads, since a definition of one is not
// yet; the identifier the expression is, through parentheses, 6.5.2p6,
// or null, for the diagnostics that name it and for the function a call
// names; and whether it is a constant reached through a signed
// overflow, 6.6p4, the mark every operator's production sets,
// overflowed(), and a constant expression reads, warn_overflowed();
// whether it is the result of * of a scalar, through parentheses, a
// place through a pointer, whose step and compound assignment are not
// yet; the token of a cast of an object to a type of its width,
// narrower than an int or an unsigned, through parentheses and the
// casts that widen it, or null, and whether it is one change of the
// object's sign alone, for the not yet of its index,
// widened_index()'s; the type of an array, or none: an expression of
// an array's type is converted to a pointer to its first element,
// 6.3.2.1p3, which is its C type here, read by every production, and
// the node is the array's decay, since no row reads an array whole, but
// for & and sizeof, which read the array's type, and so take it through
// parentheses; and the facts of rcc's tree that a member access reads,
// rcc's record_address() and record_pointer() (docs/typed-tree.md 4.2):
// for the result of *, through parentheses, the operand it was read
// through, of which &*X is X, 6.5.4.2p3; for a cast between pointer
// types, the operand under every such cast, rcc's uncast(); and for a
// sum of a pointer to an 8-bit integer and a constant, the pointer under
// its casts and the constant, rcc's is_byte_sum(); each null for any
// other operand.
struct operand_t
{
    type_t type;
    std::optional< expr_t > node;
    const token_t *name = nullptr;
    bool overflowed = false;
    bool indirect = false;
    const token_t *kept_bits = nullptr;
    bool sign_changed = false;
    std::optional< type_t > array = std::nullopt;
    std::shared_ptr< const operand_t > pointer = nullptr;
    std::shared_ptr< const operand_t > uncast = nullptr;
    std::shared_ptr< const record_pointer_t > byte_sum = nullptr;
};

// The pointer through which a member access reaches its record and the
// constant in bytes added to it, rcc's record_pointer_t.
struct record_pointer_t
{
    operand_t pointer;
    long offset;
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

// What a scope holds of a tag it declares, rcc's tag_t: the type it
// names; the keyword that declared it, struct, union or enum, of which
// every other use of the tag is, 6.7.3.4p2; whether a list has defined
// its content; and the record of a struct or a union, which the parser
// completes in place at its list's '}', p5, or null for an enum.  An
// enum's type is the enumerated type, or while its list is read the
// type of the enum in its own list.
struct tag_t
{
    type_t type;
    tok_t keyword;
    bool defined;
    record_t *record = nullptr;
};

// A scope, 6.2.1p4: the file's, a block's or a parameter list's.  It
// holds the ordinary identifiers it declares, 6.2.3p1, each a view into
// the tokens and what the scope holds of it, the file's its typedef
// names and enumeration constants alone, its functions' and its
// objects' names being parser_t's; and the tags it declares, of
// structs, unions and enums, in a name space of their own.
struct scope_t
{
    std::unordered_map< std::string_view, symbol_t > names = { };
    std::unordered_map< std::string_view, tag_t > tags = { };
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
// yet, unit_entity()'s; the count of the identities the parser has
// given the enumerated types and the types of an enum in its own list,
// 6.2.5p21, which identifies the next; and the structs and the unions
// the file declares, in a deque, whose elements never move, so a
// record_type_t made before a list completes its record points to the
// completed record, 6.7.3.4p5: no row reads a C type, so they die with
// the parse (docs/typed-tree.md principle 9).
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
    std::deque< record_t > records = { };
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

// Plain char is signed or unsigned as the target has it, and a type of
// its own, 6.2.5p20, rcc's char_type().
type_t char_type(const parser_t &p);

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

// An operand whose value an operator reads, judged before its type is,
// rcc's rvalue()'s judgements: not void, require_value()'s, and no
// record, refuse_record_operand()'s, each an error at the token.
void require_scalar(const token_t &at, const operand_t &e);

// An operand whose value is read, rcc's rvalue(): a void one is none
// and a record is none of a scalar, require_scalar()'s, each an error at
// the token; a function designator's value is its function's address,
// 6.3.2.1p4, which is not yet; any other is its node, the lvalue
// converted, 6.3.2.1p2.
expr_t rvalue(const token_t &at, operand_t e);

// An operand evaluated for its effect alone, 6.8.4p2, rcc's
// discarded(): it may be void; a record's value it would read is
// refuse_record_value()'s at the token; and a function designator,
// which would decay to its function's address, is not yet.
expr_t discarded(const token_t &at, operand_t e);

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
// require_value()'s; a record converts to no scalar, and no scalar to a
// record, require_record_conversion()'s; an integer converts to a
// pointer only as a null pointer constant, a pointer to no integer, and
// a pointer to a pointer to a compatible type alone, 6.5.17.2p1, a
// function designator being a pointer to its function, each an error at
// the token; any other value is converted to t, convert_to()'s.
// Returns the value's node.
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

// *E of the operand arg at the token op, rcc's indirection(): the place
// arg points to, of its pointee type, 6.5.4.2p4, its operand the fact
// pointer.
operand_t indirect(const token_t &op, const operand_t &arg);
operand_t assignment_expression(parser_t &);
operand_t expression(parser_t &);

// 6.7, parse-declarations.cc

// What the parser holds of the declaration specifiers: the storage
// class, static, extern, typedef or none; the first type specifier's
// token, a typedef name's or a tagged type's specifier's among them; the
// type; and whether the type specifier is a struct, union or enum
// specifier, which declares its tag, its record or its constants, so that
// a declaration of it alone, struct s; or enum e { A };, declares
// something, 6.7.1p2.
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

// 6.7.3.2, 6.7.3.4, parse-records.cc

// How a struct, union or enum specifier uses its tag, 6.7.3.4, rcc's
// tag_use_t: a list defines the tag's content, p10; struct S; declares
// the tag alone, p11; any other mention refers to the type the tag
// names, p12, p13.
enum class tag_use_t { defines, declares, refers };

// The tag after struct, union or enum, or null where the list follows
// at once, rcc's optional_tag(); a specifier with neither is an error.
const token_t *optional_tag(parser_t &, const token_t &keyword);

// The type a tag names, 6.7.3.4, rcc's tagged_type().  A list and
// struct S; declare the tag in the current scope, p10, p11, and a tag
// declared there already is the same type, p4.  Any other mention is the
// type of the innermost declaration of the tag in scope, p13, or without
// one declares an incomplete struct or union in the current scope, p12.
// The tag of another kind of type is an error, p1, and so is an enum
// without its list before the enum is declared, p2.  A second list for
// one tag in one scope is valid when it defines the same content, p1,
// which rcc does not compare, so it is not yet.  A tag first declared in
// a parameter list is declared in the list's scope, 6.2.1p4, and ends
// with it, or with the body of a definition.
type_t tagged_type(parser_t &,
                   const token_t &keyword,
                   const token_t &tag,
                   tag_use_t use);

// The tag the scope declares by the name, or null.
tag_t *tag_in(scope_t &scope, std::string_view name);

// struct or union, then a tag, a member list in braces, or both,
// 6.7.3.2p1, rcc's struct_or_union_specifier().  A tag followed by ; is
// the declaration of the tag alone, 6.7.3.4p11; without a list a tag
// names the type tagged_type() finds.
type_t struct_or_union_specifier(parser_t &);

// The words a diagnostic names a record by, rcc's record_words(): struct
// 'point', or untagged union for one declared without a tag.
std::string record_words(const record_t &r);

// The record a type is, or null, rcc's record_in().
const record_t *record_in(const type_t &t);

// A type rcc declares and does not use yet stands wherever nothing needs
// its object, and a use that needs it is refused by the type's own
// words, rcc's require_honoured(): a not-yet type, and a record whose
// list holds a construct rcc does not lay out yet.
void require_honoured(const token_t &at, const type_t &t);

// rcc compares no two records' members, so a mismatch of two types that
// holds two distinct complete records of one kind and one tag is not
// yet, rcc's refuse_tag_twins(): C23 may make the two compatible,
// 6.2.7p1.  Any other mismatch is the caller's error.
void refuse_tag_twins(const token_t &at, const type_t &a, const type_t &b);

// A record converted to the value stored in its object, 6.3.2.1p2, where
// a record is wanted or for its effect, rcc's refuse_record_value(): its
// value is not yet, and an incomplete record has none to read, an error,
// p2.
void refuse_record_value(const token_t &at, const type_t &t);

// Every operator that reads its operand's value asks a scalar of it, or
// an arithmetic or an integer type, and so does a condition, 6.5.4.3p1,
// 6.5.7p2, 6.5.14p2, 6.5.15p2, 6.5.16p2, 6.8.5.2p1, 6.8.6.1p2, rcc's
// refuse_record_operand(): a record there is the error of the
// constraint, not a construct rcc lacks.
void refuse_record_operand(const token_t &at, const type_t &t);

// No record converts as if by assignment to a scalar, and a record takes
// only a record compatible with it, 6.5.17.2p1, which rcc takes as the
// same record, rcc's require_record_conversion(): either way round the
// conversion is the error of incompatible types; two records of one tag
// are not yet.
void require_record_conversion(const token_t &at,
                               const type_t &from,
                               const type_t &to);

// An object of a record type, which rcc-v2 does not declare yet, refused
// at its name.
void refuse_record_object(const token_t &name, const type_t &t);

// The operand e as the pointer value, its node read: the facts a member
// access reads of a pointer kept, uncast and byte_sum.
operand_t pointer_operand(const operand_t &e, expr_t value);

// The member of a record by its name, 6.5.3.4p1, p2, at its offset in
// the record, rcc's named_member(): a member the record declares, or one
// of an anonymous member's at any depth, 6.7.3.2p15; a name the record
// has no member of is an error at the name.
member_t named_member(const record_t &r, const token_t &name);

// The record a member access at op reads a member of, 6.5.3.4p1, p2,
// rcc's accessed_record(): the record of the type t for E.m, and the one
// t points to for P->m, each an error at op where there is none.  It is
// complete, since its members are known from its list's '}' on, and laid
// out, or the access is refused by the words of what rcc does not lay
// out, require_honoured()'s.
const record_t &accessed_record(const token_t &op, const type_t &t);

// The lvalue of the member m of the record E is, for E.m, or of the one
// P points to, for P->m, P read for its value, op saying which,
// 6.5.3.4p3, p4, rcc's member_lvalue(): *(T *) ((char *) P + offset),
// lvalue_at()'s, of the member's type, the offset the member's in its
// record plus the one P carries, record_pointer()'s.
operand_t member_lvalue(const parser_t &p,
                        const token_t &op,
                        const operand_t &e,
                        const member_t &m);

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
