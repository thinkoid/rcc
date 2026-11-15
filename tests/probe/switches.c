/* The switches probe: the switch statement in rcc's C, for
   docs/backend.md 7.3 and docs/aarch64.md 7.3.  Per type, switches that
   take each of the rule's three forms, direct, simple and hash, and
   those with no case; the first value 0, positive and negative; values
   at the type's limits; the default first, in the middle, last and
   absent; fall-through; the rule's edges.  Then char and short values,
   promoted; nested switches; a switch in a loop and a loop in a switch;
   a case inside a nested block and inside a loop of the body; a
   controlling expression with a side effect, evaluated once; a
   register variable's value.  Each function's register variables are
   declared at its head.  See switches.h. */

#include "switches.h"

/* docs/backend.md 7.3, per type, the direct form, each switch's ncase
   the count of its cases and range the last value less the first, at
   most 3 * ncase:
   - direct_zero, the first value 0: ncase 5, range 6; the default last;
   - direct_up, the first value positive: ncase 6, range 8; the default
     first;
   - direct_down, the first value negative, for int and long: ncase 4,
     range 5; for unsigned and unsigned long the values sort as 0, 2 and
     the two at the top, and the form is simple; the default in the
     middle;
   - direct_top, at the top of the type, the cases out of order: ncase
     4, range 3; the default in the middle;
   - direct_bottom, at the bottom of the type, the least value first:
     ncase 3, range 3; no default, so a value no case holds runs no part
     of the body;
   - edge_direct, the rule's edge: ncase 4, range 12, 3 * ncase;
   - fall, fall-through, a default in the middle and a break: ncase 5,
     range 5. */
#define DIRECT(T, N, LO, HI)               \
        int direct_zero_##N(T x)           \
        {                                  \
                switch (x) {               \
                case 0: return 1;          \
                case 1: return 2;          \
                case 3: return 3;          \
                case 5: return 4;          \
                case 6: return 5;          \
                default: return 6;         \
                }                          \
        }                                  \
                                           \
        int direct_up_##N(T x)             \
        {                                  \
                switch (x) {               \
                default: return 7;         \
                case 100: return 1;        \
                case 101: return 2;        \
                case 102: return 3;        \
                case 104: return 4;        \
                case 107: return 5;        \
                case 108: return 6;        \
                }                          \
        }                                  \
                                           \
        int direct_down_##N(T x)           \
        {                                  \
                switch (x) {               \
                case -3: return 1;         \
                case -2: return 2;         \
                default: return 3;         \
                case 0: return 4;          \
                case 2: return 5;          \
                }                          \
        }                                  \
                                           \
        int direct_top_##N(T x)            \
        {                                  \
                switch (x) {               \
                case HI - 3: return 1;     \
                case HI: return 2;         \
                default: return 3;         \
                case HI - 1: return 4;     \
                case HI - 2: return 5;     \
                }                          \
        }                                  \
                                           \
        int direct_bottom_##N(T x)         \
        {                                  \
                int r = 9;                 \
                                           \
                switch (x) {               \
                case LO: r = 1; break;     \
                case LO + 1: r = 2; break; \
                case LO + 3: r = 3; break; \
                }                          \
                                           \
                return r;                  \
        }                                  \
                                           \
        int edge_direct_##N(T x)           \
        {                                  \
                switch (x) {               \
                case 0: return 1;          \
                case 4: return 2;          \
                case 8: return 3;          \
                case 12: return 4;         \
                }                          \
                                           \
                return 5;                  \
        }                                  \
                                           \
        int fall_##N(T x)                  \
        {                                  \
                int r = 0;                 \
                                           \
                switch (x) {               \
                case 1: r += 1;            \
                case 2: r += 10;           \
                default: r += 100;         \
                case 4: r += 1000; break;  \
                case 5: r += 10000;        \
                case 6: r += 100000;       \
                }                          \
                                           \
                return r;                  \
        }

LIMITS(DIRECT)

/* docs/backend.md 7.3, per type, the simple form, below ten cases and
   not direct:
   - edge_simple, one above the rule's edge: ncase 4, range 13;
   - simple, 0, a positive and a negative value: ncase 4, the range
     above 12 for every type; the default in the middle;
   - simple_limits, the least and the greatest values and 1: ncase 3,
     the range the whole type's; no default;
   - single, a single case: ncase 1, range 0, which the rule never makes
     direct;
   - nine, the most cases the form takes: ncase 9. */
#define SIMPLE(T, N, LO, HI)           \
        int edge_simple_##N(T x)       \
        {                              \
                switch (x) {           \
                case 0: return 1;      \
                case 4: return 2;      \
                case 8: return 3;      \
                case 13: return 4;     \
                }                      \
                                       \
                return 5;              \
        }                              \
                                       \
        int simple_##N(T x)            \
        {                              \
                switch (x) {           \
                case 5000: return 1;   \
                case -7: return 2;     \
                default: return 3;     \
                case 0: return 4;      \
                case 70000: return 5;  \
                }                      \
        }                              \
                                       \
        int simple_limits_##N(T x)     \
        {                              \
                int r = 0;             \
                                       \
                switch (x) {           \
                case HI: r = 1; break; \
                case LO: r = 2; break; \
                case 1: r = 3; break;  \
                }                      \
                                       \
                return r;              \
        }                              \
                                       \
        int single_##N(T x)            \
        {                              \
                switch (x) {           \
                case 42: return 1;     \
                }                      \
                                       \
                return 2;              \
        }                              \
                                       \
        int nine_##N(T x)              \
        {                              \
                switch (x) {           \
                case 1: return 1;      \
                case 20: return 2;     \
                case 300: return 3;    \
                case 4000: return 4;   \
                case -5: return 5;     \
                case 60000: return 6;  \
                case 700000: return 7; \
                case -80: return 8;    \
                case 9: return 9;      \
                }                      \
                                       \
                return 10;             \
        }

LIMITS(SIMPLE)

/* docs/backend.md 7.3, per type, no case: the value is computed, and
   control goes past the body, whose statement no label reaches, in
   none; or to the default, which every value reaches, in only_default */
#define NONE(T, N, LO, HI)        \
        int none_##N(T x)         \
        {                         \
                int r = 1;        \
                                  \
                switch (x) {      \
                        r = 2;    \
                }                 \
                                  \
                return r;         \
        }                         \
                                  \
        int only_default_##N(T x) \
        {                         \
                int r = 1;        \
                                  \
                switch (x) {      \
                default: r = 2;   \
                }                 \
                                  \
                return r;         \
        }

LIMITS(NONE)

/* docs/backend.md 7.3, per type, the hash form, ten cases or more and
   not direct: ncase 13, and the range far above 39; 0, the limits and
   negative values among the cases, the default in the middle.  The
   table's size is the i from 3 to 6 the rule picks for the type's bits
   of the values. */
#define HASH(T, N, LO, HI)                 \
        int hash_##N(T x)                  \
        {                                  \
                switch (x) {               \
                case 0: return 1;          \
                case 1: return 2;          \
                case 17: return 3;         \
                case 300: return 4;        \
                case 4096: return 5;       \
                case 65537: return 6;      \
                case -2: return 7;         \
                case -100: return 8;       \
                case HI: return 9;         \
                case LO + 5: return 10;    \
                case 1000000: return 11;   \
                case 77: return 12;        \
                default: return 13;        \
                case 123456789: return 14; \
                }                          \
        }

LIMITS(HASH)

/* docs/backend.md 7.3 and chapter 4, per type: a register variable's
   value, read and never written, in each form: direct with the first
   value 0, ncase 3, range 3; direct above it, ncase 3, range 3; simple,
   ncase 3; hash, ncase 10.  The result says whether the variable kept
   its value. */
#define REGISTER(T, N, LO, HI)                      \
        int register_##N(T a)                       \
        {                                           \
                register T x = a;                   \
                register int r = 0;                 \
                                                    \
                switch (x) {                        \
                case 0: r = 1; break;               \
                case 2: r = 2; break;               \
                case 3: r = 3; break;               \
                }                                   \
                switch (x) {                        \
                case 10: r = r + 10; break;         \
                case 11: r = r + 20; break;         \
                case 13: r = r + 30; break;         \
                }                                   \
                switch (x) {                        \
                case 7: r = r + 100; break;         \
                case -9: r = r + 200; break;        \
                case 30000: r = r + 300; break;     \
                }                                   \
                switch (x) {                        \
                case 3: r = r + 1000; break;        \
                case 50: r = r + 2000; break;       \
                case 700: r = r + 3000; break;      \
                case 9000: r = r + 4000; break;     \
                case -1: r = r + 5000; break;       \
                case -20: r = r + 6000; break;      \
                case 123: r = r + 7000; break;      \
                case 4567: r = r + 8000; break;     \
                case 88888: r = r + 9000; break;    \
                case 1000000: r = r + 10000; break; \
                }                                   \
                                                    \
                return r * 2 + (x == a);            \
        }

LIMITS(REGISTER)

/* char and short values, promoted to int, 6.8.5.3p5: a case matches
   where the type holds its value.  Simple, ncase 7; then direct, ncase
   5, range 4. */
#define NARROW_SWITCHES(T, N)          \
        int narrow_##N(T c)            \
        {                              \
                switch (c) {           \
                case -1: return 1;     \
                case 0: return 2;      \
                case 'a': return 3;    \
                case 127: return 4;    \
                case 200: return 5;    \
                case 65535: return 6;  \
                case -32768: return 7; \
                }                      \
                                       \
                return 8;              \
        }                              \
                                       \
        int letters_##N(T c)           \
        {                              \
                switch (c) {           \
                case 'a': return 1;    \
                case 'b': return 2;    \
                case 'c': return 3;    \
                case 'd': return 4;    \
                case 'e': return 5;    \
                }                      \
                                       \
                return 6;              \
        }

NARROW(NARROW_SWITCHES)

/* nested switches: the inner one's cases repeat the outer's values and
   it has a default of its own, 6.8.5.3p3; its break leaves it alone */
int
nested(int x, int y)
{
        int r = 0;

        switch (x) {
        case 1:
                switch (y) {
                case 1: r = 11; break;
                case 2: r = 12;
                default: r += 100;
                }
                r += 1000;
                break;
        case 2:
                switch (y) {
                case 2: r = 22; break;
                }
        case 3: r += 3; break;
        default:
                switch (y) {
                default: r = 99;
                }
        }

        return r;
}

/* a switch in a loop: its break leaves the switch, and a continue goes
   on with the loop, to the for's step and to the while's condition.  k
   bounds the passes, so a continue to the wrong place shows in the
   result and does not hang. */
long
in_loop(int n)
{
        long s = 0;
        int k = 0;

        for (int i = 0; i < n; i++) {
                if (++k > 100)
                        break;

                switch (i % 5) {
                case 0: s += 1; break;
                case 1: continue;
                case 2: s += 100;
                case 3: s += 10; break;
                default:
                        if (i > 12)
                                continue;
                        s += 1000;
                }

                s += 7;
        }

        int i = 0;
        while (i < n) {
                if (++k > 200)
                        break;

                switch (i++ & 3) {
                case 1: continue;
                case 2: break;
                default: s += 30000;
                }

                s += 500000;
        }

        return s * 1000 + k;
}

/* a loop in a switch: its break leaves the loop, not the switch */
int
loop_in_switch(int x)
{
        int r = 0;

        switch (x & 3) {
        case 0:
                for (int i = 0;; i++) {
                        if (i > (x & 15))
                                break;
                        r += i;
                }
                r += 1000;
        case 1:
                while (1) {
                        r += 2;
                        if (r > 50)
                                break;
                }
                break;
        default:
                do {
                        r += 3;
                        if (r > 20)
                                break;
                } while (1);
        }

        return r;
}

/* a case inside a nested block of the body, past the declaration of
   an object, which is assigned before it is read */
int
nested_block(int x)
{
        int r = 0;

        switch (x) {
        case 1: {
                int y = x * 3;

                r = y;
        case 2:
                y = x + 40;
                r += y;
                if (x == 2)
                        break;
        }
        case 3: r += 500; break;
        case 4: r = 4;
        }

        return r;
}

/* a case before a declaration, 6.8.2p4, and a default before the
   body's closing brace */
int
labels(int x)
{
        int r = 1;

        switch (x) {
        case 1:
                int k = x * 7;
                r = k;
                break;
        case 2: r = 2;
        default:
        }

        return r;
}

/* Duff's device: the cases inside the do loop of the body, count
   elements copied eight at a time from the case the remainder picks */
int
duff(int count)
{
        int from[20];
        int to[20];
        int *f = from;
        int *t = to;
        int s = 0;
        int n;
        int i;

        for (i = 0; i < 20; i++) {
                from[i] = i * 3 + 1;
                to[i] = 0;
        }

        if (count <= 0 || count > 20)
                return -1;

        n = (count + 7) / 8;
        switch (count % 8) {
        case 0: do { *t++ = *f++;
                case 7: *t++ = *f++;
                case 6: *t++ = *f++;
                case 5: *t++ = *f++;
                case 4: *t++ = *f++;
                case 3: *t++ = *f++;
                case 2: *t++ = *f++;
                case 1: *t++ = *f++;
                } while (--n > 0);
        }

        for (i = 0; i < 20; i++)
                s += to[i] * (i + 1);

        return s * 100 + (int) (t - to);
}

/* a controlling expression with a side effect, evaluated once: a call
   that counts its calls, and a ++ in its argument */
int calls;

int
counted(int v)
{
        calls++;

        return v;
}

int
once(int x)
{
        int k = x;
        int r = 0;

        calls = 0;
        switch (counted(k++)) {
        case 1: r = 1; break;
        case 2: r = 2; break;
        case 3: r = 3; break;
        case 5: r = 5;
        }

        return r * 100 + calls * 10 + (k - x);
}
