// -*- mode: c++; -*-

// 6.7.3.2 and 6.7.3.4: the struct and union specifiers, their member
// lists and their layout, the tags of structs, unions and enums, and
// the rules C states of a record where an expression is one, rcc's.
// The layout is clang's on each target, as rcc's lay_out() makes it,
// ported from rcc's parser, where it lives.

#include "parse-common.hh"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rcc {

//
// the words and the refusals of a record
//

std::string
record_words(const record_t &r)
{
    const std::string kind = r.is_union ? "union" : "struct";
    if (r.tag.empty())
        return "untagged " + kind;

    return kind + " " + quoted(r.tag);
}

const record_t *
record_in(const type_t &t)
{
    return is< record_type_t >(t) ? as< record_type_t >(t).record : nullptr;
}

namespace {

// The words of what rcc does not use yet that a type is: a not-yet
// type's, or those of a record whose list holds a construct rcc does
// not lay out yet; null for any other type.
const refusal_words_t *
not_yet_in(const type_t &t)
{
    if (is< not_yet_t >(t))
        return &as< not_yet_t >(t).what;

    if (const record_t *r = record_in(t); r && !r->not_yet.words.empty())
        return &r->not_yet;

    return nullptr;
}

} // namespace

void
require_honoured(const token_t &at, const type_t &t)
{
    if (const refusal_words_t *refusal = not_yet_in(t))
        not_yet(at, refusal->words);
}

namespace {

// Two distinct complete records of one kind and one tag, which C23
// makes compatible when their members agree, 6.2.7p1, where the C of
// before made them two types: the first such pair met walking two
// types alike, through pointers and functions, or null, rcc's
// tag_twin().
const record_t *
tag_twin(const type_t &a, const type_t &b)
{
    if (is< record_type_t >(a) && is< record_type_t >(b)) {
        const record_t &x = *as< record_type_t >(a).record;
        const record_t &y = *as< record_type_t >(b).record;
        const bool twins = &x != &y && x.is_union == y.is_union && x.complete &&
                           y.complete && !x.tag.empty() && x.tag == y.tag;

        return twins ? &x : nullptr;
    }

    if (is< pointer_t >(a) && is< pointer_t >(b))
        return tag_twin(pointee_of(a), pointee_of(b));

    if (!is< function_t >(a) || !is< function_t >(b))
        return nullptr;

    const auto &f = as< function_t >(a);
    const auto &g = as< function_t >(b);
    if (const record_t *r = tag_twin(*f.result, *g.result))
        return r;

    for (std::size_t i = 0; i < std::min(f.params.size(), g.params.size()); ++i)
        if (const record_t *r = tag_twin(*f.params[i], *g.params[i]))
            return r;

    return nullptr;
}

} // namespace

void
refuse_tag_twins(const token_t &at, const type_t &a, const type_t &b)
{
    if (const record_t *r = tag_twin(a, b))
        not_yet(at, "compatibility of two types of " + record_words(*r));
}

namespace {

// A record whose object is read or written whole: complete, 6.3.2.1p1,
// p2, an error otherwise, and laid out, not yet otherwise, rcc's
// require_whole_record().
void
require_whole_record(const token_t &at, const type_t &t)
{
    const record_t &r = *record_in(t);
    if (!r.complete)
        fail(at, "incomplete type " + record_words(r) +
                     " where a complete type is required");

    require_honoured(at, t);
}

} // namespace

void
refuse_record_value(const token_t &at, const type_t &t)
{
    if (!record_in(t))
        return;

    require_whole_record(at, t);
    not_yet(at, "value of " + record_words(*record_in(t)));
}

void
refuse_record_operand(const token_t &at, const type_t &t)
{
    if (const record_t *r = record_in(t))
        fail(at,
             "value of " + record_words(*r) + " where a scalar is required");
}

namespace {

// The words a conversion's error names a type by, rcc's
// conversion_words(): a record by its own, a scalar by its kind.
std::string
conversion_words(const type_t &t)
{
    if (const record_t *r = record_in(t))
        return record_words(*r);

    return is< pointer_t >(t) ? "pointer" : "integer";
}

} // namespace

void
require_record_conversion(const token_t &at,
                          const type_t &from,
                          const type_t &to)
{
    if (!record_in(from) && !record_in(to))
        return;

    refuse_tag_twins(at, to, from);
    if (from != to)
        fail(at, "incompatible " + conversion_words(from) + " to " +
                     conversion_words(to) + " conversion");
}

void
refuse_record_object(const token_t &name, const type_t &t)
{
    if (const record_t *r = record_in(t))
        not_yet(name,
                "object " + quoted(name.text) + " of " + record_words(*r));
}

//
// the tags
//

namespace {

// A new record of the file's, incomplete until a list completes it,
// 6.7.3.4p5, of the kind the keyword says and the tag, empty for none,
// rcc's new_record().
record_t &
new_record(parser_t &p, const token_t &keyword, std::string_view tag)
{
    p.records.push_back(
        record_t { keyword.kind == tok_t::kw_union, std::string(tag) });

    return p.records.back();
}

// A tag met first, declared in the current scope as a new type of an
// identity of its own, 6.7.3.4p9, rcc's new_tag(): a struct or a union
// a record, new_record()'s; an enum a not-yet type until its list makes
// it the integer type compatible with it.
tag_t &
new_tag(parser_t &p, const token_t &keyword, const token_t &tag)
{
    tag_t fresh { make_void(), keyword.kind, false };
    if (keyword.kind != tok_t::kw_enum)
        fresh.record = &new_record(p, keyword, tag.text);

    fresh.type = fresh.record ? make_record(*fresh.record) :
                                make_not_yet(++p.identities, { "enum" });

    return p.scopes.back().tags.emplace(tag.text, fresh).first->second;
}

} // namespace

tag_t *
tag_in(scope_t &scope, std::string_view name)
{
    const auto it = scope.tags.find(name);

    return it == scope.tags.end() ? nullptr : &it->second;
}

type_t
tagged_type(parser_t &p,
            const token_t &keyword,
            const token_t &tag,
            tag_use_t use)
{
    tag_t *found = tag_in(p.scopes.back(), tag.text);

    if (use == tag_use_t::refers)
        for (auto scope = p.scopes.rbegin(); !found && scope != p.scopes.rend();
             ++scope)
            found = tag_in(*scope, tag.text);

    if (found && found->keyword != keyword.kind)
        fail(tag,
             "use of " + quoted(tag.text) +
                 " with tag type that does not match previous declaration");

    if (!found && use == tag_use_t::refers && keyword.kind == tok_t::kw_enum)
        fail(tag, "enum " + quoted(tag.text) + " used before its list");

    if (found && use == tag_use_t::defines && found->defined)
        not_yet(tag, std::string(keyword.text) + " " + quoted(tag.text) +
                         " defined again");

    if (!found)
        found = &new_tag(p, keyword, tag);

    if (use == tag_use_t::defines)
        found->defined = true;

    return found->type;
}

const token_t *
optional_tag(parser_t &p, const token_t &keyword)
{
    if (peek(p).kind == tok_t::lbrace)
        return nullptr;

    return &expect(p, tok_t::identifier,
                   "expected identifier or '{' after " + quoted(keyword.text));
}

//
// the member list
//

namespace {

// Words that say a construct of a record's list rcc does not lay out
// yet, kept in the record, rcc's mark_not_yet(): the first one met is
// the one its uses are refused by, require_honoured()'s.
void
mark_not_yet(record_t &r, refusal_words_t refusal)
{
    if (r.not_yet.words.empty())
        r.not_yet = std::move(refusal);
}

// GNU C's attributes after a member's declarator or a bit-field's width
// are said of the member, which rcc-v2 refuses where they stand.
void
refuse_member_attribute(const parser_t &p)
{
    if (peek(p).kind == tok_t::kw_attribute)
        not_yet(peek(p), quoted(peek(p).text) + " after a declarator");
}

// The width of a bit-field after its ':', an integer constant
// expression, 6.7.3.2p4, of a bit-field whose type is an integer type,
// p5, or one rcc does not use yet, rcc's bit_field().  The width is not
// negative, does not exceed the type's bits, and is zero only where the
// field has no name, p4; a type rcc does not use yet has no width to
// exceed.  One reached through a signed overflow draws the warning of
// 6.6p4, warn_overflowed()'s.  name is the field's, or null.  Returns
// the width.
int
bit_field(parser_t &p, const type_t &t, const token_t *name)
{
    const token_t &colon = advance(p);
    if (!is< int_t >(t) && !is< not_yet_t >(t))
        fail(colon, "bit-field of a type that is not an integer type");

    const token_t &start = peek(p);
    const operand_t width = conditional_expression(p);
    const std::optional< long long > value = constant_of(rvalue(start, width));
    if (!value || !is< int_t >(width.type))
        fail(start, "bit-field width is not an integer constant expression");

    warn_overflowed(p, start, width);

    const std::string field =
        name ? "bit-field " + quoted(name->text) : "unnamed bit-field";
    const exact_t bits = exact_value(width.type, *value);
    if (bits < 0)
        fail(start, field + " of negative width");

    if (bits == 0 && name)
        fail(start, "named " + field + " of zero width");

    const int most = is< int_t >(t) ? as< int_t >(t).width : 0;
    if (most > 0 && bits > most)
        fail(start,
             field + " of " +
                 std::to_string(static_cast< unsigned long long >(bits)) +
                 " bits, wider than its type's " + std::to_string(most));

    refuse_member_attribute(p);

    return int(std::min(bits, exact_t(std::numeric_limits< int >::max())));
}

// A member's type: a complete object type, 6.7.3.2p11, so neither a
// function, p3, nor void, nor an incomplete record, the record being
// defined among them, which holds no instance of itself, p3, rcc's
// require_member_type().  A type rcc does not use yet, a record that
// holds a construct rcc does not lay out among them, leaves the record
// without its layout, refused by the member type's words.
void
require_member_type(const token_t &name, const type_t &t, record_t &r)
{
    if (is< function_t >(t))
        fail(name, "member " + quoted(name.text) + " declared as a function");

    if (is< void_t >(t))
        fail(name, "member " + quoted(name.text) + " has type void");

    if (const record_t *inner = record_in(t); inner && !inner->complete)
        fail(name, "member " + quoted(name.text) + " has incomplete type " +
                       record_words(*inner));

    if (const refusal_words_t *refusal = not_yet_in(t))
        mark_not_yet(r, *refusal);
}

// The names of a record's members, its named bit-fields and its
// anonymous members' at any depth among them, 6.7.3.2p15, rcc's
// member_names().
std::vector< std::string >
member_names(const record_t &r)
{
    std::vector< std::string > names;

    for (const member_t &m : r.members) {
        if (!m.name.empty()) {
            names.push_back(m.name);
            continue;
        }

        if (!is_anonymous(m))
            continue;

        const auto inner = member_names(*record_in(m.type));
        names.insert(names.end(), inner.begin(), inner.end());
    }

    return names;
}

// The member of the name in a record, at its offset from the start of
// the record, or none, rcc's member_named(): a member the record
// declares, or one of an anonymous member's, at any depth, at the
// anonymous member's offset plus its own, since its members are the
// record's, 6.7.3.2p15, reached through it.  A named bit-field is found
// as any member is; an unnamed one, p14, names nothing.
std::optional< member_t >
member_named(const record_t &r, std::string_view name)
{
    for (const member_t &m : r.members) {
        if (m.name == name)
            return m;

        if (!is_anonymous(m))
            continue;

        auto inner = member_named(*record_in(m.type), name);
        if (inner) {
            inner->offset += m.offset;
            return inner;
        }
    }

    return std::nullopt;
}

// A named member, its type checked and its name one the record has not
// declared yet, its anonymous members' and its bit-fields' among them,
// 6.7.3.2p1, p15 and 6.2.3p1: each record is a name space of its own,
// rcc's add_member().
void
add_member(record_t &r, const token_t &name, const type_t &t)
{
    require_member_type(name, t, r);

    if (member_named(r, name.text))
        fail(name, "duplicate member " + quoted(name.text));

    r.members.push_back(member_t { std::string(name.text), t, 0 });
}

// An anonymous member, 6.7.3.2p15, rcc's anonymous_member(): a member
// of no name and of the record of the specifier, laid out as a member
// of its type is, as clang lays it out, so its own layout is kept.  Its
// members are the enclosing record's: each name is one the record has
// not declared yet, refused at the ';', since the member's own list was
// read before it was known to be anonymous.  A record rcc does not lay
// out leaves the enclosing one without its layout, by its words.
void
anonymous_member(const token_t &semicolon, record_t &r, const type_t &t)
{
    for (const std::string &name : member_names(*record_in(t)))
        if (member_named(r, name))
            fail(semicolon, "duplicate member " + quoted(name));

    if (const refusal_words_t *refusal = not_yet_in(t))
        mark_not_yet(r, *refusal);

    r.members.push_back(member_t { { }, t, 0 });
}

// A bit-field of the width, named or not, 6.7.3.2p11, p14, rcc's
// add_bit_field(): a member, kept for the layout, lay_out()'s.  A named
// one's name is one the record has not declared yet, p1, p15 and
// 6.2.3p1.  A field of a type rcc does not use yet leaves the record
// without its layout, by the type's words, as a member of the type
// does.
void
add_bit_field(record_t &r, const token_t *name, const type_t &t, int width)
{
    if (name && member_named(r, name->text))
        fail(*name, "duplicate member " + quoted(name->text));

    if (const refusal_words_t *refusal = not_yet_in(t))
        mark_not_yet(r, *refusal);

    const std::string named = name ? std::string(name->text) : std::string();
    r.members.push_back(member_t { named, t, 0, { }, width });
}

// A member of an array type, a flexible array member among them, whose
// rules rcc's member list keeps, is not yet: refused at the first '['
// of its declarator, before the array's size is judged, since rcc reads
// a last member's [0] as a flexible array member, or at its name where
// a typedef name made the array.
void
refuse_member_array(const parser_t &p)
{
    std::size_t ahead = 0;
    while (peek(p, ahead).kind == tok_t::star)
        ++ahead;

    if (peek(p, ahead).kind == tok_t::identifier &&
        peek(p, ahead + 1).kind == tok_t::lbracket)
        not_yet(peek(p, ahead + 1), "array member");
}

// One member declarator, 6.7.3.2p1, rcc's member_declarator(): a
// declarator, which names the member, then a bit-field's width or none;
// or a width alone, an unnamed bit-field, p14.
void
member_declarator(parser_t &p, const specifiers_t &spec, record_t &r)
{
    if (peek(p).kind == tok_t::colon) {
        const int width = bit_field(p, spec.type, nullptr);
        add_bit_field(r, nullptr, spec.type, width);
        return;
    }

    refuse_member_array(p);
    const declarator_t d = declarator(p, spec.type);
    if (is< array_t >(d.type))
        not_yet(d.name, "array member");

    refuse_member_attribute(p);

    if (peek(p).kind == tok_t::colon) {
        const int width = bit_field(p, d.type, &d.name);
        add_bit_field(r, &d.name, d.type, width);
        return;
    }

    add_member(r, d.name, d.type);
}

// A member declaration with no declarator, 6.7.3.2p2, rcc's
// unnamed_member(): a struct or a union specifier without a tag
// declares an anonymous member, p15.  Any other struct, union or enum
// specifier declares its tag or its constants alone, as at file scope;
// one of no specifier of the kind declares nothing, an error.
void
unnamed_member(const token_t &semicolon, const specifiers_t &spec, record_t &r)
{
    if (!spec.tagged)
        fail(semicolon, "declaration does not declare anything");

    const record_t *inner = record_in(spec.type);
    if (inner && inner->tag.empty())
        anonymous_member(semicolon, r, spec.type);
}

// A member declaration, rcc's member_declaration(): its specifiers,
// which are a specifier-qualifier list, 6.7.3.2p1, with no storage
// class, and its member declarators.
void
member_declaration(parser_t &p, record_t &r)
{
    const specifiers_t spec = declaration_specifiers(p);

    if (const token_t *t = spec.storage_class)
        fail(*t,
             "storage class " + quoted(t->text) + " in a member declaration");

    if (peek(p).kind == tok_t::semicolon) {
        unnamed_member(advance(p), spec, r);
        return;
    }

    do
        member_declarator(p, spec, r);
    while (accept(p, tok_t::comma));

    expect(p, tok_t::semicolon, "expected ';' at end of member declaration");
}

// The member declarations of a list, up to its '}', 6.7.3.2p1, rcc's
// member_list().  The list opens no scope: a tag or an enumeration
// constant declared in it is declared where the record is, 6.2.1p4, and
// the members' names are the record's own, 6.2.3p1.  A list of no member
// at all makes an empty record, whose behavior 6.7.3.2p10 leaves
// undefined and GNU C takes, which rcc does not lay out yet.  rcc reads a
// static assertion, 6.7.12, and GNU C's __extension__ before a member
// declaration, and rcc-v2 refuses either where it stands.
void
member_list(parser_t &p, record_t &r)
{
    while (!next_is(p, { tok_t::rbrace, tok_t::eof })) {
        const token_t &t = peek(p);
        if (t.kind == tok_t::kw_extension || t.kind == tok_t::kw_static_assert)
            not_yet(t, quoted(t.text) + " in a member list");

        member_declaration(p, r);
    }

    if (r.members.empty())
        mark_not_yet(r, { "empty " + record_words(r) });
}

//
// the layout
//

// The next free bit of a struct as its layout goes: the byte, and the
// count of its bits, 0 to 7, that the bit-fields before took, rcc's
// next_bit_t.
struct next_bit_t
{
    long byte = 0;
    int bit = 0;
};

// The access unit of a named bit-field, its container's offset and its
// first bit in the container in hand, rcc's access_unit(): the
// narrowest unit of 1, 2, 4 or 8 bytes, at a multiple of its size from
// the record's start, that holds the field's bits.  Its offset, its
// size and the field's first bit in it become the member's (docs/shape.md
// chapter 3).  Units of sizes that are powers of two nest, and the
// container is one, so the unit lies in the container.
void
access_unit(member_t &m)
{
    const int size = int(size_of(m.type));

    for (int unit = 1; unit <= size; unit *= 2) {
        const int start = m.bit / (8 * unit) * unit;
        if (m.bit + m.width <= (start + unit) * 8) {
            m.offset += start;
            m.unit = unit;
            m.bit -= start * 8;
            return;
        }
    }

    throw std::logic_error("v2: a bit-field past its container");
}

// The place of a bit-field, 6.7.3.2p13, p14, in its container, the unit
// of its declared type, of the type's size at a multiple of it from the
// record's start, rcc's bit_field_place(): the next free bit, next,
// where the rest of that container holds the field's width, and the
// next container's first bit where it does not, so no field straddles
// two; a width of 0 moves to the next container's first bit, unless next
// is one.  Fields fill a container from its lowest bit up.  In a union
// next stays at the record's start, so every field is at bit 0, p18.  A
// named field takes its access unit, access_unit()'s.  Returns the end
// of the field's bits, in bytes; close is the list's '}', where a record
// too large for a size is refused.
long
bit_field_place(const token_t &close,
                const record_t &r,
                member_t &m,
                next_bit_t &next)
{
    constexpr long largest = std::numeric_limits< long >::max();
    const long size = size_of(m.type);

    if (next.byte > largest - 2 * size)
        fail(close, record_words(r) + " is too large");

    long container = next.byte / size * size;
    int bit = int(next.byte - container) * 8 + next.bit;

    const bool moves = m.width == 0 ? bit > 0 : bit + m.width > size * 8;
    if (moves) {
        container += size;
        bit = 0;
    }

    m.offset = container;
    m.bit = bit;
    if (!m.name.empty())
        access_unit(m);

    if (!r.is_union)
        next = { container + (bit + m.width) / 8, (bit + m.width) % 8 };

    return container + (bit + m.width + 7) / 8;
}

// The layout of a record its list completed, as clang lays it out on
// the target, rcc's lay_out(): each member of a struct at the offset so
// far rounded up to the member's alignment, in the order declared,
// 6.7.3.2p16, p17; each member of a union at 0, p18; the record's
// alignment its members' largest, and its size the end of its members
// rounded up to that alignment, p19, so that each element of an array
// of it is aligned.  A bit-field takes its bits, bit_field_place()'s,
// and the member after it the next byte past them, at its own
// alignment, so it may lie in the field's container; the end of a field
// is the end of its last bit.  A field's declared type counts in the
// record's alignment, an unnamed field's only where the target says
// so.  close is the list's '}', where a record too large for a size is
// refused.
void
lay_out(const token_t &close, record_t &r, const target_t &target)
{
    constexpr long largest = std::numeric_limits< long >::max();

    next_bit_t next;
    long end = 0;
    int alignment = 1;

    for (member_t &m : r.members) {
        if (is_bit_field(m)) {
            end = std::max(end, bit_field_place(close, r, m, next));
            if (!m.name.empty() || target.unnamed_field_aligns)
                alignment = std::max(alignment, align_of(m.type));

            continue;
        }

        const long size = size_of(m.type);
        const int align = align_of(m.type);

        if (end > largest - align)
            fail(close, record_words(r) + " is too large");

        m.offset = r.is_union ? 0 : (end + align - 1) / align * align;

        if (size > largest - m.offset)
            fail(close, record_words(r) + " is too large");

        if (!r.is_union)
            next = { m.offset + size, 0 };

        end = std::max(end, m.offset + size);
        alignment = std::max(alignment, align);
    }

    if (end > largest - alignment)
        fail(close, record_words(r) + " is too large");

    r.size = (end + alignment - 1) / alignment * alignment;
    r.alignment = alignment;
}

} // namespace

// The list completes the record in place, 6.7.3.4p5, and lays it out
// unless it holds a construct rcc does not lay out yet: a #pragma pack
// that sets a packing where the list opens changes the layout, which
// rcc does not honour yet (shape.md chapter 2).  GNU C's attributes
// after the keyword are not yet, and so are those after the list, which
// the specifiers refuse.
type_t
struct_or_union_specifier(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (peek(p).kind == tok_t::kw_attribute)
        not_yet(peek(p), quoted(peek(p).text) + " in a " +
                             std::string(keyword.text) + " specifier");

    const token_t *tag = optional_tag(p, keyword);

    if (peek(p).kind != tok_t::lbrace) {
        const tag_use_t use = peek(p).kind == tok_t::semicolon ?
                                  tag_use_t::declares :
                                  tag_use_t::refers;

        return tagged_type(p, keyword, *tag, use);
    }

    if (tag)
        tagged_type(p, keyword, *tag, tag_use_t::defines);

    record_t &r = tag ? *tag_in(p.scopes.back(), tag->text)->record :
                        new_record(p, keyword, { });

    const token_t &open = advance(p);
    member_list(p, r);
    const token_t &close =
        expect(p, tok_t::rbrace, "expected '}' at end of member list");

    if (open.packed)
        mark_not_yet(r, { "pragma 'pack' on " + record_words(r) });

    if (r.not_yet.words.empty())
        lay_out(close, r, p.target);

    r.complete = true;

    return make_record(r);
}

//
// the member access
//

operand_t
pointer_operand(const operand_t &e, expr_t value)
{
    return { .type = e.type,
             .node = std::move(value),
             .uncast = e.uncast,
             .byte_sum = e.byte_sum };
}

member_t
named_member(const record_t &r, const token_t &name)
{
    if (auto m = member_named(r, name.text))
        return std::move(*m);

    fail(name,
         "no member named " + quoted(name.text) + " in " + record_words(r));
}

const record_t &
accessed_record(const token_t &op, const type_t &t)
{
    const bool arrow = op.kind == tok_t::arrow;
    if (arrow && !is< pointer_t >(t))
        fail(op, "member reference type is not a pointer");

    const type_t &r = arrow ? pointee_of(t) : t;
    if (!record_in(r))
        fail(op, arrow ? "member reference type is not a pointer to a "
                         "struct or a union" :
                         "member reference base type is not a struct or a "
                         "union");

    const record_t &record = *record_in(r);
    if (!record.complete)
        fail(op, "member access into incomplete type " + record_words(record));

    require_honoured(op, r);

    return record;
}

namespace {

// The address of the record of E.m, (&E)->m, 6.5.3.4p4's footnote, rcc's
// record_address(): E is *X, and &*X is X, 6.5.4.2p3, the operand's fact
// pointer; any other record is a value, no lvalue, whose member is not
// yet, said at the '.'.
const operand_t &
record_address(const token_t &op, const operand_t &e)
{
    if (!e.pointer)
        not_yet(op, "member of a value of " + record_words(*record_in(e.type)));

    return *e.pointer;
}

// The pointer of P->m, or of E.m, record_address()'s, and the constant
// in bytes added to it, rcc's record_pointer(): a cast between pointer
// types changes no address and is read through, and so is a sum of a
// pointer to bytes and a constant, whose constant is taken out, so that
// the members of a member, the tree a member access makes, are one
// offset from the outermost pointer.  Each is a fact of the operand,
// uncast and byte_sum.
record_pointer_t
record_pointer(const operand_t &pointer)
{
    const operand_t &e = pointer.uncast ? *pointer.uncast : pointer;
    if (e.byte_sum)
        return *e.byte_sum;

    return { e, 0 };
}

// The lvalue of type t at offset bytes from what pointer points to,
// *(T *) ((char *) pointer + offset), rcc's lvalue_at(), of the
// builders' own nodes, as the Seventh Edition compiler's first pass
// wrote a->b as *(a + offset): the cast to char * where the pointer is
// no pointer to char in the machine's view, the sum by the offset, at the
// stride 1, the cast to T *, each a node only where the builders make
// one, then *, indirect()'s at op.  At offset 0 it is *(T *) pointer,
// with no sum.  Each operand made keeps the facts rcc reads off its
// tree: the sum its byte_sum, the pointer under its casts and the
// offset, and the cast its uncast.
operand_t
lvalue_at(const parser_t &p,
          const token_t &op,
          const operand_t &pointer,
          long offset,
          const type_t &t)
{
    operand_t at = pointer;

    if (offset != 0) {
        const type_t bytes = make_pointer(char_type(p));
        const operand_t &under = pointer.uncast ? *pointer.uncast : pointer;
        expr_t base =
            pointer.type == bytes ? *pointer.node : make_bitcast(*pointer.node);
        at = { .type = bytes,
               .node = make_gep(binary_op_t::add, std::move(base),
                                make_literal(offset, view_t::i64), 1, false),
               .byte_sum = std::make_shared< const record_pointer_t >(
                   record_pointer_t { under, offset }) };
    }

    const type_t to = make_pointer(t);
    if (at.type != to) {
        auto under =
            at.uncast ? at.uncast : std::make_shared< const operand_t >(at);
        at = { .type = to,
               .node = make_bitcast(*at.node),
               .uncast = std::move(under) };
    }

    return indirect(op, at);
}

} // namespace

operand_t
member_lvalue(const parser_t &p,
              const token_t &op,
              const operand_t &e,
              const member_t &m)
{
    const record_pointer_t at =
        record_pointer(op.kind == tok_t::dot ? record_address(op, e) : e);

    return lvalue_at(p, op, at.pointer, at.offset + m.offset, m.type);
}

} // namespace rcc
