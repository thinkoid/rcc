// -*- mode: c++; -*-

// The facts of a target that the front end, the generators and the
// driver all read: its two names, six numbers, the words of its frame
// limit and its variable argument list.  Everything else about a
// target is its generator's.  docs/aarch64.md chapter 11.

#pragma once

#include <initializer_list>
#include <string>
#include <string_view>

namespace rcc {

// The variable argument list of a target's convention, 7.16p4, as the
// C library reads it: the x86-64 psABI's, an array of one record of
// the offsets into the register save area and the two areas'
// addresses, or the AAPCS64's, a record of the stack area's address,
// the tops of the two register areas and the offsets below them.
enum class va_list_t { psabi, aapcs64 };

// name is the target as -target spells it, clang_name the triple clang
// takes after --target=.  register_variables is the count of variables
// a register declaration can place.  char_is_signed and
// wchar_is_signed are the signedness of plain char and of wchar_t, a
// 32-bit integer on both targets, as clang has them.  frame_limit is
// the largest frame in bytes, a multiple of 16, and
// frame_limit_spelled is it in words.  array_alignment is
// the convention's for an array object at file scope, which code
// another compiler writes for its declaration may assume: an array of
// that many bytes or more is aligned to at least that many, and 1 is no
// rule.  va_list is the convention's list.  unnamed_field_aligns says
// whether an unnamed bit-field's type counts in the alignment of its
// record, as clang lays the record out (docs/shape.md chapter 3).
struct target_t
{
    std::string_view name;
    std::string_view clang_name;
    int register_variables;
    bool char_is_signed;
    bool wchar_is_signed;
    long frame_limit;
    std::string_view frame_limit_spelled;
    int array_alignment;
    va_list_t va_list;
    bool unnamed_field_aligns;
};

// The frame is the 32-bit immediate of sub, and a slot a 32-bit
// displacement off rbp: 2 GiB less 16, docs/backend.md chapter 5.  A
// global array of 16 bytes or more is aligned to 16, by the psABI's
// "Aggregates and Unions", backend.md chapter 10.
inline constexpr target_t x86_64 { .name = "x86-64",
                                   .clang_name = "x86_64-linux-gnu",
                                   .register_variables = 5,
                                   .char_is_signed = true,
                                   .wchar_is_signed = true,
                                   .frame_limit = (1L << 31) - 16,
                                   .frame_limit_spelled = "2 GiB less 16 bytes",
                                   .array_alignment = 16,
                                   .va_list = va_list_t::psabi,
                                   .unnamed_field_aligns = false };

// 16 MiB less 16, docs/aarch64.md 5.2: a frame past it is an error.
// The AAPCS64 aligns an array as its element, chapter 10.
inline constexpr target_t aarch64 { .name = "aarch64",
                                    .clang_name = "aarch64-linux-gnu",
                                    .register_variables = 10,
                                    .char_is_signed = false,
                                    .wchar_is_signed = false,
                                    .frame_limit = (1L << 24) - 16,
                                    .frame_limit_spelled =
                                        "16 MiB less 16 bytes",
                                    .array_alignment = 1,
                                    .va_list = va_list_t::aapcs64,
                                    .unnamed_field_aligns = true };

// The limit of a frame as the two errors that refuse one past it say
// it, the parser's for a local and c1's for a whole frame: the limit is
// rcc's for the target, not one of C (docs/choices.md).
inline std::string
frame_limit_words(const target_t &target)
{
    return "rcc's frame on " + std::string(target.name) + " holds at most " +
           std::string(target.frame_limit_spelled);
}

// The target of a -target name, or null for a name that is none.
constexpr const target_t *
target_named(std::string_view name)
{
    for (const target_t *target : { &x86_64, &aarch64 })
        if (target->name == name)
            return target;

    return nullptr;
}

// The machine rcc was built on, the driver's default.
constexpr const target_t &
build_target()
{
#if defined(__x86_64__)
    return x86_64;
#elif defined(__aarch64__)
    return aarch64;
#else
#  error "rcc builds on x86-64 and aarch64 only"
#endif
}

} // namespace rcc
