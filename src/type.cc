// -*- mode: c++; -*-

// The type constructors, structural equality and C's relations declared
// in type.hh.  Equality walks the edges: two types made apart compare
// equal when they spell the same machine type.  The relations walk them
// alike and read what C remembers beside the machine's view.

#include "type.hh"

#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

namespace rcc {

namespace {

type_ptr_t
share(type_t t)
{
    return std::make_shared< const type_t >(std::move(t));
}

bool
equal(const void_t &, const void_t &)
{
    return true;
}

bool
equal(const int_t &a, const int_t &b)
{
    return a.width == b.width && a.is_signed == b.is_signed;
}

bool
equal(const pointer_t &a, const pointer_t &b)
{
    return *a.pointee == *b.pointee;
}

bool
equal(const array_t &a, const array_t &b)
{
    return a.count == b.count && a.length == b.length &&
           *a.element == *b.element;
}

bool
equal(const record_type_t &a, const record_type_t &b)
{
    return a.record == b.record;
}

bool
equal(const not_yet_t &a, const not_yet_t &b)
{
    return a.identity == b.identity;
}

// The parameter lists compare through the edges; comparing the
// vectors directly would compare the shared pointers.
bool
equal(const function_t &a, const function_t &b)
{
    const auto same = [](const type_ptr_t &x, const type_ptr_t &y) {
        return *x == *y;
    };

    return a.variadic == b.variadic && *a.result == *b.result &&
           std::equal(a.params.begin(), a.params.end(), b.params.begin(),
                      b.params.end(), same);
}

// The two relations of C's view: compatible types, and the same type,
// which asks of an enumerated type its own identity and of an array its
// count.
enum class relation_t { compatible, same };

bool related(const type_t &, const type_t &, relation_t);

bool
related(const void_t &a, const void_t &b, relation_t)
{
    return equal(a, b);
}

// By kind, 6.2.5p20: char, signed char and unsigned char are three
// types, long and long long two.  An enumerated type has the kind of
// the integer type compatible with it, 6.7.3.3p13, and is compatible
// with no other enumerated type; it is the same type as itself alone.
bool
related(const int_t &a, const int_t &b, relation_t r)
{
    if (a.kind != b.kind)
        return false;

    if (a.enumeration == b.enumeration)
        return true;

    return r == relation_t::compatible &&
           (a.enumeration == 0 || b.enumeration == 0);
}

// By the pointees, 6.7.7.2p2, each qualified as the pointer says: two
// qualified types are compatible when their qualifiers are the same,
// 6.7.4.1p11, so const char * and char * are not, and neither are
// const char ** and char **.
bool
related(const pointer_t &a, const pointer_t &b, relation_t r)
{
    return a.qualifiers == b.qualifiers && related(*a.pointee, *b.pointee, r);
}

// By the elements, and by the counts where both are constant, 6.7.7.3p6;
// the same type has the same count, or the same length.
bool
related(const array_t &a, const array_t &b, relation_t r)
{
    if (r == relation_t::same && (a.count != b.count || a.length != b.length))
        return false;

    const bool sized = a.count >= 0 && !a.length && b.count >= 0 && !b.length;
    if (sized && a.count != b.count)
        return false;

    return related(*a.element, *b.element, r);
}

bool
related(const record_type_t &a, const record_type_t &b, relation_t)
{
    return equal(a, b);
}

bool
related(const not_yet_t &a, const not_yet_t &b, relation_t)
{
    return equal(a, b);
}

// By the results and the parameters, 6.7.7.4p14, which the type keeps
// adjusted and unqualified already.
bool
related(const function_t &a, const function_t &b, relation_t r)
{
    const auto each = [r](const type_ptr_t &x, const type_ptr_t &y) {
        return related(*x, *y, r);
    };

    return a.variadic == b.variadic && related(*a.result, *b.result, r) &&
           std::equal(a.params.begin(), a.params.end(), b.params.begin(),
                      b.params.end(), each);
}

bool
related(const type_t &a, const type_t &b, relation_t r)
{
    if (a.v.index() != b.v.index())
        return false;

    return std::visit(
        [&](const auto &x) {
            return related(x, std::get< std::decay_t< decltype(x) > >(b.v), r);
        },
        a.v);
}

} // namespace

bool
operator==(const type_t &a, const type_t &b)
{
    if (a.v.index() != b.v.index())
        return false;

    return std::visit(
        [&](const auto &x) {
            return equal(x, std::get< std::decay_t< decltype(x) > >(b.v));
        },
        a.v);
}

bool
compatible(const type_t &a, const type_t &b)
{
    return related(a, b, relation_t::compatible);
}

bool
same_type(const type_t &a, const type_t &b)
{
    return related(a, b, relation_t::same);
}

// 6.2.7p3, through the edges: an array of the count the two know, of
// the composite element; a pointer to the composite pointee, the first
// one's qualifiers kept; a function of the composite result and
// parameters, the first one's marking kept.  Of an enumerated type and
// its integer type it is the integer type, whichever is first, as clang
// has it, the choice p3 leaves to the implementation (choices.md).
// Otherwise the two are one type, and it is the first.  No variable
// length array meets another here: none has linkage, and a pointer to
// one is not yet.
type_t
composite(const type_t &a, const type_t &b)
{
    if (is< int_t >(a) && as< int_t >(a).enumeration != 0)
        return b;

    if (is< pointer_t >(a))
        return make_pointer(
            composite(*as< pointer_t >(a).pointee, *as< pointer_t >(b).pointee),
            as< pointer_t >(a).qualifiers);

    if (is< array_t >(a)) {
        const array_t &x = as< array_t >(a);
        const array_t &y = as< array_t >(b);
        const long count = x.count >= 0 ? x.count : y.count;

        return make_array(composite(*x.element, *y.element), count);
    }

    if (!is< function_t >(a))
        return a;

    const function_t &f = as< function_t >(a);
    const function_t &g = as< function_t >(b);

    std::vector< type_t > params;
    for (std::size_t i = 0; i < f.params.size(); ++i)
        params.push_back(composite(*f.params[i], *g.params[i]));

    return make_function(composite(*f.result, *g.result), std::move(params),
                         f.variadic, f.marking);
}

type_t
make_void()
{
    return type_t { void_t { } };
}

type_t
make_int(int width, bool is_signed, int_kind_t kind, int enumeration)
{
    return type_t { int_t { width, is_signed, kind, enumeration } };
}

type_t
make_pointer(type_t pointee, qualifiers_t qualifiers)
{
    return type_t { pointer_t { share(std::move(pointee)), qualifiers } };
}

type_t
make_array(type_t element, long count)
{
    return type_t { array_t { share(std::move(element)), count } };
}

type_t
make_variable_array(type_t element, const symbol_t &length)
{
    return type_t { array_t { share(std::move(element)), 0, &length } };
}

type_t
make_function(type_t result,
              std::vector< type_t > params,
              bool variadic,
              std::string marking)
{
    std::vector< type_ptr_t > edges;
    edges.reserve(params.size());

    for (auto &param : params)
        edges.push_back(share(std::move(param)));

    return type_t { function_t { share(std::move(result)), std::move(edges),
                                 variadic, std::move(marking) } };
}

type_t
make_record(const record_t &r)
{
    return type_t { record_type_t { &r } };
}

type_t
make_not_yet(int identity, std::string what, bool made)
{
    return type_t { not_yet_t { identity, std::move(what), 0, 0, nullptr,
                                made } };
}

// The type a typedef names through an array size rcc cannot compute,
// which keeps the array's element.
type_t
make_not_yet_array(int identity, std::string what, type_t element)
{
    return type_t { not_yet_t { identity, std::move(what), 0, 0,
                                share(std::move(element)), true } };
}

// A real floating type is aligned to its size on both targets.
type_t
make_floating(int identity, std::string what, long size)
{
    return type_t { not_yet_t { identity, std::move(what), size, int(size) } };
}

// An array of unknown size has no size, and a variable length array
// none but at run time; asking is the caller's error.  void has GNU C's
// size, 1, the step of arithmetic on a pointer to it; the parser
// refuses sizeof (void) before it asks.
long
size_of(const type_t &t)
{
    if (is< void_t >(t))
        return 1;

    if (is< int_t >(t))
        return as< int_t >(t).width / 8;

    if (is< pointer_t >(t))
        return 8;

    if (is< array_t >(t) && as< array_t >(t).count >= 0 &&
        !as< array_t >(t).length)
        return as< array_t >(t).count * size_of(*as< array_t >(t).element);

    if (is< record_type_t >(t) && laid_out(*as< record_type_t >(t).record))
        return as< record_type_t >(t).record->size;

    if (is< not_yet_t >(t) && as< not_yet_t >(t).size > 0)
        return as< not_yet_t >(t).size;

    throw std::logic_error("size_of: a type without a size");
}

int
align_of(const type_t &t)
{
    if (is< int_t >(t))
        return as< int_t >(t).width / 8;

    if (is< pointer_t >(t))
        return 8;

    if (is< array_t >(t))
        return align_of(*as< array_t >(t).element);

    if (is< record_type_t >(t) && laid_out(*as< record_type_t >(t).record))
        return as< record_type_t >(t).record->alignment;

    if (is< not_yet_t >(t) && as< not_yet_t >(t).alignment > 0)
        return as< not_yet_t >(t).alignment;

    throw std::logic_error("align_of: a type without an alignment");
}

} // namespace rcc
