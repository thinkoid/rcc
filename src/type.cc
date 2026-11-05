// -*- mode: c++; -*-

// The type constructors and structural equality declared in type.hh.
// Equality walks the edges: two types made apart compare equal when
// they spell the same machine type.

#include "type.hh"

#include <algorithm>
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
    return a.count == b.count && *a.element == *b.element;
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

type_t
make_void()
{
    return type_t { void_t { } };
}

type_t
make_int(int width, bool is_signed)
{
    return type_t { int_t { width, is_signed } };
}

type_t
make_pointer(type_t pointee)
{
    return type_t { pointer_t { share(std::move(pointee)) } };
}

type_t
make_array(type_t element, long count)
{
    return type_t { array_t { share(std::move(element)), count } };
}

type_t
make_function(type_t result, std::vector< type_t > params, bool variadic)
{
    std::vector< type_ptr_t > edges;
    edges.reserve(params.size());

    for (auto &param : params)
        edges.push_back(share(std::move(param)));

    return type_t { function_t { share(std::move(result)), std::move(edges),
                                 variadic } };
}

} // namespace rcc
