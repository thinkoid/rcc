// -*- mode: c++; -*-

// The two targets as a handle's interface names them: each target's
// state of generation and its address of an object in memory, and what
// aarch64's address of a pointer reads, declared and not defined
// (docs/typed-tree.md 4.7).  A handle names every target in its
// interface, so whoever holds one sees these names, and only amd64.hh
// and arm64.hh, which define them, see more of a target.  The
// namespaces are the targets' own, since x86_64 and aarch64 are
// target.hh's objects.

#pragma once

namespace rcc::amd64 {

struct state_t;
struct address_t;

} // namespace rcc::amd64

namespace rcc::arm64 {

struct state_t;
struct address_t;
enum class between_t;
struct index_t;
struct denoted_t;

} // namespace rcc::arm64
