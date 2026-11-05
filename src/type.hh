// -*- mode: c++; -*-

// The C types as the machine sees them.  A type is a value; the edges
// to the types it names are shared and never written after
// construction, so copying is cheap and sharing is invisible.  The C
// spelling is not kept: two integer types of one width and signedness
// are one type here.

#pragma once

#include <memory>
#include <variant>
#include <vector>

namespace rcc {

struct type_t;

using type_ptr_t = std::shared_ptr< const type_t >;

struct void_t
{
};

// width in bits
struct int_t
{
    int width;
    bool is_signed;
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

struct type_t
{
    std::variant< void_t, int_t, pointer_t, array_t, function_t > v;
};

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

// Structural: two pointer types are equal when their pointees are.
bool operator==(const type_t &, const type_t &);

type_t make_void();
type_t make_int(int width, bool is_signed);
type_t make_pointer(type_t pointee);
type_t make_array(type_t element, long count);
type_t
make_function(type_t result, std::vector< type_t > params, bool variadic);

} // namespace rcc
