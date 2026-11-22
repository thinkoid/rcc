/* The lists the fields probe is generated from.  fields.c makes the
   functions of each record, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_; fields-main.c calls both over byte
   patterns and compares.  The records are defined here, so the main,
   rcc's build and clang's see one set.

   A record holds a bit-field f of each integer type and each width it
   takes, at bit 0 of its container, at bit 1 past an unnamed field of
   one bit, and at the container's top past an unnamed field of the
   rest, alone, before a char and after one; and _Bool's and an
   enumerated type's fields.  A field is read by the parser's shifts of
   its access unit and stored into by its masks, and one that fills its
   unit is an object of the unit's type, docs/shape.md chapter 5.

   struct order holds two fields of one unit, for the order of a store:
   its value is computed before the unit is loaded, so a store the
   right operand makes into the other field, or into the same one,
   stays where the field's own bits do not cover it. */

/* the enumerated type of a bit-field, of unsigned int */
enum two_bits { TB0, TB1, TB2, TB3 };

/* clang-format off */
/* the three places of a field of type T, of B bits, and width W less
   than B: a name, the type, the width and the unnamed field before it */
#define PLACES(X, N, T, B, W)                                           \
    X(N##W##_b0, T, W, ) X(N##W##_b1, T, W, T : 1;)                     \
    X(N##W##_top, T, W, T : (B - W);)

/* a field as wide as its type, which has one place */
#define FULL(X, N, T, B) X(N##B##_b0, T, B, )

/* the widths below 8, 16, 32 and 64 */
#define W7(X, N, T, B)                                                  \
    PLACES(X, N, T, B, 1) PLACES(X, N, T, B, 2) PLACES(X, N, T, B, 3)   \
    PLACES(X, N, T, B, 4) PLACES(X, N, T, B, 5) PLACES(X, N, T, B, 6)   \
    PLACES(X, N, T, B, 7)

#define W15(X, N, T, B)                                                 \
    W7(X, N, T, B) PLACES(X, N, T, B, 8) PLACES(X, N, T, B, 9)          \
    PLACES(X, N, T, B, 10) PLACES(X, N, T, B, 11)                       \
    PLACES(X, N, T, B, 12) PLACES(X, N, T, B, 13)                       \
    PLACES(X, N, T, B, 14) PLACES(X, N, T, B, 15)

#define W31(X, N, T, B)                                                 \
    W15(X, N, T, B) PLACES(X, N, T, B, 16) PLACES(X, N, T, B, 17)       \
    PLACES(X, N, T, B, 18) PLACES(X, N, T, B, 19)                       \
    PLACES(X, N, T, B, 20) PLACES(X, N, T, B, 21)                       \
    PLACES(X, N, T, B, 22) PLACES(X, N, T, B, 23)                       \
    PLACES(X, N, T, B, 24) PLACES(X, N, T, B, 25)                       \
    PLACES(X, N, T, B, 26) PLACES(X, N, T, B, 27)                       \
    PLACES(X, N, T, B, 28) PLACES(X, N, T, B, 29)                       \
    PLACES(X, N, T, B, 30) PLACES(X, N, T, B, 31)

#define W63(X, N, T, B)                                                 \
    W31(X, N, T, B) PLACES(X, N, T, B, 32) PLACES(X, N, T, B, 33)       \
    PLACES(X, N, T, B, 34) PLACES(X, N, T, B, 35)                       \
    PLACES(X, N, T, B, 36) PLACES(X, N, T, B, 37)                       \
    PLACES(X, N, T, B, 38) PLACES(X, N, T, B, 39)                       \
    PLACES(X, N, T, B, 40) PLACES(X, N, T, B, 41)                       \
    PLACES(X, N, T, B, 42) PLACES(X, N, T, B, 43)                       \
    PLACES(X, N, T, B, 44) PLACES(X, N, T, B, 45)                       \
    PLACES(X, N, T, B, 46) PLACES(X, N, T, B, 47)                       \
    PLACES(X, N, T, B, 48) PLACES(X, N, T, B, 49)                       \
    PLACES(X, N, T, B, 50) PLACES(X, N, T, B, 51)                       \
    PLACES(X, N, T, B, 52) PLACES(X, N, T, B, 53)                       \
    PLACES(X, N, T, B, 54) PLACES(X, N, T, B, 55)                       \
    PLACES(X, N, T, B, 56) PLACES(X, N, T, B, 57)                       \
    PLACES(X, N, T, B, 58) PLACES(X, N, T, B, 59)                       \
    PLACES(X, N, T, B, 60) PLACES(X, N, T, B, 61)                       \
    PLACES(X, N, T, B, 62) PLACES(X, N, T, B, 63)

/* every field: a name, the type, the width and the unnamed field
   before it */
#define CASES(X)                                                        \
    W7(X, c, char, 8) FULL(X, c, char, 8) W7(X, a, signed char, 8)      \
    FULL(X, a, signed char, 8) W7(X, h, unsigned char, 8)               \
    FULL(X, h, unsigned char, 8) W15(X, s, short, 16)                   \
    FULL(X, s, short, 16) W15(X, t, unsigned short, 16)                 \
    FULL(X, t, unsigned short, 16) W31(X, i, int, 32)                   \
    FULL(X, i, int, 32) W31(X, u, unsigned, 32)                         \
    FULL(X, u, unsigned, 32) W63(X, l, long, 64) FULL(X, l, long, 64)   \
    W63(X, m, unsigned long, 64) FULL(X, m, unsigned long, 64)          \
    W63(X, q, long long, 64) FULL(X, q, long long, 64)                  \
    W63(X, r, unsigned long long, 64)                                   \
    FULL(X, r, unsigned long long, 64) X(b1_b0, _Bool, 1, )             \
    X(b1_b1, _Bool, 1, _Bool : 1;)                                      \
    X(b1_top, _Bool, 1, unsigned char : 7;)                             \
    X(e2_b0, enum two_bits, 2, )                                        \
    X(e2_b1, enum two_bits, 2, enum two_bits : 1;)                      \
    X(e2_top, enum two_bits, 2, enum two_bits : 30;)                    \
    X(e32_b0, enum two_bits, 32, )

/* the three shapes of each field: the record's name and its members */
#define SHAPES(Y, N, T, W, PAD)                                         \
    Y(N##_alone, PAD T f : W;) Y(N##_before, PAD T f : W; char n;)     \
    Y(N##_after, char c; PAD T f : W;)
/* clang-format on */

/* the records */
#define RECORD(N, MEMBERS) \
        struct N           \
        {                  \
                MEMBERS    \
        };

#define RECORDS(N, T, W, PAD) SHAPES(RECORD, N, T, W, PAD)

CASES(RECORDS)

struct order
{
        unsigned a : 3;
        int f : 5;
};
