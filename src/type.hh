// -*- mode: c++; -*-

// The C types as the machine sees them.  A type is a value; the edges
// to the types it names are shared and never written after
// construction, so copying is cheap and sharing is invisible.
// Equality is the machine's view: two integer types of one width and
// signedness are one type to it, whatever C type each remembers.

#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace rcc {

struct type_t;
struct record_t;

using type_ptr_t = std::shared_ptr< const type_t >;

struct void_t
{
};

// The C type an integer type is, of the standard ones, 6.2.5p4, p6,
// p20: char, signed char and unsigned char are three of one width, and
// long and long long two.  none is a type c1 makes for itself.
enum class int_kind_t {
    none,
    char_,
    signed_char,
    unsigned_char,
    short_,
    unsigned_short,
    int_,
    unsigned_,
    long_,
    unsigned_long,
    long_long,
    unsigned_long_long
};

// width in bits.  kind is the C type the parser made it as, and
// enumeration the identity of the enumerated type it is, 6.2.5p21, or
// 0, its kind then that of its compatible integer type.  Equality reads
// neither, and nothing does yet.
struct int_t
{
    int width;
    bool is_signed;
    int_kind_t kind = int_kind_t::none;
    int enumeration = 0;
};

struct pointer_t
{
    type_ptr_t pointee;
};

// A count of -1 is an array of unknown size.
struct array_t
{
    type_ptr_t element;
    long count;
};

struct function_t
{
    type_ptr_t result;
    std::vector< type_ptr_t > params;
    bool variadic;
};

// A struct or a union, 6.7.3.2: the record its declarations describe,
// which the unit owns, so that a pointer made before the members are
// known points to the type they complete, 6.7.3.4p5.  Two records are
// one type when they are one record: one tag in one scope declares one
// type, p4, and each specifier with a list and no tag one of its own,
// p9.
struct record_type_t
{
    const record_t *record;
};

// A type rcc declares and does not use yet: an enum in its own list or
// marked by an attribute, a floating type, _Bool, __builtin_va_list, an
// atomic type, the type a typedef names through a marking attribute or
// through an array size rcc cannot compute.  Any other enum is the
// integer type compatible with it, an int_t.  Two not-yet types are one
// type when their identities are equal.  what is the words a refusal
// prints.  A pointer to it is an ordinary pointer.  It has no size and
// no alignment, size and alignment 0, but for a real floating type,
// which a record lays out as a member (shape.md chapter 3): its size
// and alignment are then clang's.  element is the element of the array
// a typedef names through a size rcc cannot compute, which a parameter
// of the type is adjusted to a pointer to, 6.7.7.4p6; null for any
// other.  made is whether the type was made from another by a typedef
// through a marking attribute or an array size rcc cannot compute:
// rcc does not know what the attribute or the size makes of that
// type, so it cannot tell whether another type is compatible with it.
// An atomic type is compatible with none but itself, 6.7.4.1p11, so
// _Atomic makes none.  Equality reads none of the three.
struct not_yet_t
{
    int identity;
    std::string what;
    long size = 0;
    int alignment = 0;
    type_ptr_t element = nullptr;
    bool made = false;
};

struct type_t
{
    std::variant< void_t,
                  int_t,
                  pointer_t,
                  array_t,
                  function_t,
                  record_type_t,
                  not_yet_t >
        v;
};

// A member of a record: its name, its type and its offset in bytes
// from the start of the record, 6.7.3.2p17.
struct member_t
{
    std::string name;
    type_t type;
    long offset;
};

// A struct or a union as its declarations describe it, 6.7.3.2.  The
// parser makes one incomplete at the first mention of its tag, or at a
// list without one, and completes it in place at the list's '}',
// 6.7.3.4p5; nothing writes it after the parse.  tag is empty for a
// record declared without one.  A complete record has its members, its
// size and its alignment, laid out by the rule clang has for both
// targets, unless its list holds a construct rcc does not lay out yet:
// not_yet is then the words its use is refused with, and the record has
// no size and no alignment, as a not-yet type has none.  flexible says
// that the record is a struct with a flexible array member, or a union
// with a member of such a record, possibly through other unions, which
// 6.7.3.2p3 keeps out of a struct and out of an array.
struct record_t
{
    bool is_union;
    std::string tag;
    bool complete = false;
    std::vector< member_t > members = { };
    long size = 0;
    int alignment = 0;
    std::string not_yet = { };
    bool flexible = false;
};

// Whether a record has its layout: complete, and of no construct rcc
// does not lay out yet.
inline bool
laid_out(const record_t &r)
{
    return r.complete && r.not_yet.empty();
}

template< typename T >
bool
is(const type_t &t)
{
    return std::holds_alternative< T >(t.v);
}

template< typename T >
const T &
as(const type_t &t)
{
    return std::get< T >(t.v);
}

// Structural: two pointer types are equal when their pointees are.  A
// record is equal to itself alone, so the walk stops there, and a
// record whose members point to it compares.
bool operator==(const type_t &, const type_t &);

// The size and the alignment in bytes, the same on both targets, the
// sizes LP64's: an integer its width, a pointer 8, an array its count
// times its element's and its element's alignment, and a record its
// layout's, and a real floating type clang's, the size for both: 4 for
// float, 8 for double and 16 for long double.  A void, function or
// other not-yet type has neither, and neither has a record without its
// layout; asking throws.
long size_of(const type_t &);
int align_of(const type_t &);

type_t make_void();
type_t make_int(int width,
                bool is_signed,
                int_kind_t kind = int_kind_t::none,
                int enumeration = 0);
type_t make_pointer(type_t pointee);
type_t make_array(type_t element, long count);
type_t
make_function(type_t result, std::vector< type_t > params, bool variadic);
type_t make_record(const record_t &);
type_t make_not_yet(int identity, std::string what, bool made = false);
type_t make_not_yet_array(int identity, std::string what, type_t element);
type_t make_floating(int identity, std::string what, long size);

} // namespace rcc
