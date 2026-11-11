// -*- mode: c++; -*-

// The facts of a target that the front end, the generators and the
// driver all read: its two names and three numbers.  Everything else
// about a target is its generator's.  docs/aarch64.md chapter 11.

#pragma once

#include <initializer_list>
#include <string_view>

namespace rcc {

// name is the target as -target spells it, clang_name the triple clang
// takes after --target=.  register_variables is the count of variables
// a register declaration can place, frame_limit the largest frame in
// bytes, a multiple of 16.
struct target_t
{
    std::string_view name;
    std::string_view clang_name;
    int register_variables;
    bool char_is_signed;
    long frame_limit;
};

// The frame is the 32-bit immediate of sub, and a slot a 32-bit
// displacement off rbp: 2 GiB less 16, docs/backend.md chapter 5.
inline constexpr target_t x86_64 { .name = "x86-64",
                                   .clang_name = "x86_64-linux-gnu",
                                   .register_variables = 5,
                                   .char_is_signed = true,
                                   .frame_limit = (1L << 31) - 16 };

// 16 MiB less 16, docs/aarch64.md 5.2: a frame of 16 MiB or more is
// not yet.
inline constexpr target_t aarch64 { .name = "aarch64",
                                    .clang_name = "aarch64-linux-gnu",
                                    .register_variables = 10,
                                    .char_is_signed = false,
                                    .frame_limit = (1L << 24) - 16 };

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
