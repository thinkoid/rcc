/* The enums probe: enumerated types and enumeration constants in rcc's
   C, docs/shape.md chapters 3 and 5.  Per enumeration of enums.h, an
   object, a parameter and a result of its type, conversions to it and
   from it, its signedness in a comparison, arithmetic with an integer
   and with its own type, sizeof, a pointer to it, an array of it, a
   global of it, a compound assignment and a step on it.  Per constant,
   its value, its size and its signedness.  Then a switch on each type
   by its constants, constants in a constant expression, an array size,
   an initializer and a case label, and the scope of a constant.  See
   enums.h. */

#include "enums.h"

/* the functions of one enumerated type T, its letter N */
#define TYPED(T, N)                                   \
        T id_##N(T x)                                 \
        {                                             \
                return x;                             \
        }                                             \
                                                      \
        long widen_##N(T x)                           \
        {                                             \
                return x;                             \
        }                                             \
                                                      \
        T cast_##N(long x)                            \
        {                                             \
                return (T) x;                         \
        }                                             \
                                                      \
        long assign_##N(long x)                       \
        {                                             \
                T e;                                  \
                e = x;                                \
                return e;                             \
        }                                             \
                                                      \
        int negative_##N(T x)                         \
        {                                             \
                return x < 0;                         \
        }                                             \
                                                      \
        int below_##N(T x)                            \
        {                                             \
                return x - 1 < 0;                     \
        }                                             \
                                                      \
        long add_##N(T x, int y)                      \
        {                                             \
                return x + y;                         \
        }                                             \
                                                      \
        long sub_##N(T x, T y)                        \
        {                                             \
                return x - y;                         \
        }                                             \
                                                      \
        int less_##N(T x, T y)                        \
        {                                             \
                return x < y;                         \
        }                                             \
                                                      \
        long shift_##N(T x)                           \
        {                                             \
                return x >> 1;                        \
        }                                             \
                                                      \
        long size_##N(T x)                            \
        {                                             \
                return sizeof x * 10 + sizeof(T);     \
        }                                             \
                                                      \
        long pointer_##N(long x)                      \
        {                                             \
                T e = x;                              \
                T *p = &e;                            \
                *p += 3;                              \
                (*p)++;                               \
                return *p;                            \
        }                                             \
                                                      \
        long array_##N(long x)                        \
        {                                             \
                T a[3];                               \
                T *p = a;                             \
                a[0] = x;                             \
                a[1] = a[0] * 2;                      \
                p[2] = -a[1];                         \
                return (long) (&a[2] - p) + a[2] +    \
                       ((char *) &a[1] - (char *) a); \
        }                                             \
                                                      \
        T global_##N;                                 \
                                                      \
        long global_rw_##N(long x)                    \
        {                                             \
                global_##N = x;                       \
                global_##N <<= 1;                     \
                return global_##N;                    \
        }                                             \
                                                      \
        long register_##N(long x)                     \
        {                                             \
                register T r = x;                     \
                r = r + 5;                            \
                return r;                             \
        }

ENUMS(TYPED)

/* the value, the size and the signedness of each constant */
#define CONSTANT(K)                   \
        long value_##K(void)          \
        {                             \
                return K;             \
        }                             \
                                      \
        long size_##K(void)           \
        {                             \
                return sizeof K;      \
        }                             \
                                      \
        int negative_##K(void)        \
        {                             \
                return K - K - 1 < 0; \
        }

CONSTANTS(CONSTANT)

/* a switch on each type by its constants, the case values converted
   to the promoted type, 6.8.5.3p5 */
int
which_p(enum pos x)
{
        switch (x) {
        case P0: return 1;
        case P1: return 2;
        case P2: return 3;
        case P3: return 4;
        default: return 0;
        }
}

int
which_n(enum neg x)
{
        switch (x) {
        case N0: return 1;
        case N1: return 2;
        case N2: return 3;
        default: return 0;
        }
}

int
which_b(enum big x)
{
        switch (x) {
        case BG0: return 1;
        case BG1: return 2;
        case BG2: return 3;
        default: return 0;
        }
}

int
which_bn(enum bigneg x)
{
        switch (x) {
        case BN0: return 1;
        case BN1: return 2;
        default: return 0;
        }
}

int
which_h(enum huge x)
{
        switch (x) {
        case H0: return 1;
        case H1: return 2;
        case H2: return 3;
        default: return 0;
        }
}

int
which_hn(enum hugeneg x)
{
        switch (x) {
        case HN0: return 1;
        case HN1: return 2;
        case HN2: return 3;
        default: return 0;
        }
}

int
which_a(enum after x)
{
        switch (x) {
        case A0: return 1;
        case A1: return 2;
        case A2: return 3;
        default: return 0;
        }
}

int
which_c(enum chars x)
{
        switch (x) {
        case C0: return 1;
        case C1: return 2;
        case C2: return 3;
        case C3: return 4;
        default: return 0;
        }
}

int
which_d(enum during x)
{
        switch (x) {
        case D0: return 1;
        case D1: return 2;
        case D2: return 3;
        case D3: return 4;
        case D4: return 5;
        default: return 0;
        }
}

/* constants in the expressions of the case labels of an int switch */
int
label(int x)
{
        switch (x) {
        case P2 + 1: return 1;
        case N0 * 2: return 2;
        case C0 | C1: return 3;
        case sizeof(enum huge) * 3: return 4;
        default: return 0;
        }
}

/* constants as an array's size, as a global's initializer and in a
   table of them */
int sized[P3 + N2];
enum neg initialized = N1;
enum huge top = H2;
long table[] = { P2, N0, BG1, BN0, H2, HN2, A1, C2, D2 };

long
sizes(void)
{
        return (long) sizeof sized * 100 + (long) sizeof(int[D3]);
}

long
initial(int i)
{
        return i == 0 ? (long) initialized : i == 1 ? (long) top : table[i - 2];
}

/* a constant's scope begins after its enumerator, 6.2.1p7: the inner
   O0 is the outer O0 plus one, and O1 the inner O0's next */
enum outer { O0 = 4 };

long
scope(void)
{
        enum { O0 = O0 + 1, O1 };

        return O0 * 10 + O1;
}

/* a tag and constants a parameter list declares, which the body sees,
   6.9.2p10 */
int listed(enum listed { L0 = 3, L1 } x)
{
        enum listed y = L1;

        return x * 10 + y;
}

/* an untagged enumeration through a typedef, enums.h's, and a pointer
   to it */
long
kinds(kind_t k, kind_t *p)
{
        *p = k == T1 ? T2 : T0;

        return *p + (long) sizeof(kind_t) * 10;
}
