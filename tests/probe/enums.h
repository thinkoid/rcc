/* The lists the enums probe is generated from.  enums.c makes functions
   of each entry, compiled by rcc and, as the reference, by clang -O0
   under the prefix ref_; enums-main.c calls both over a table of values
   and compares.  The enumerations are defined here, so the main, rcc's
   build and clang's see one set.

   An enumerated type is the integer type compatible with it, and an
   enumeration constant is its number, docs/shape.md chapters 3 and 5:
   the type of each enumeration below is the one clang chooses, and so
   is the type of each constant, 6.7.3.3p12, p13 and p15. */

/* unsigned int, its constants int: no value is negative */
enum pos { P0, P1, P2 = 7, P3 };

/* int, its constants int: a value is negative */
enum neg { N0 = -3, N1, N2 = 5 };

/* unsigned int, its constants unsigned int: int does not hold BG1 */
enum big { BG0, BG1 = 0x80000000, BG2 };

/* long, its constants long: a value is negative, and int does not hold
   BN1 */
enum bigneg { BN0 = -1, BN1 = 0x80000000 };

/* unsigned long, its constants unsigned long */
enum huge { H0, H1 = 0x100000000, H2 = 0xffffffffffffffff };

/* long, its constants long */
enum hugeneg { HN0 = -2, HN1 = 0x100000000, HN2 };

/* unsigned int, its constants unsigned int: A1 is INT_MAX plus one,
   long while the list is read */
enum after { A0 = 0x7fffffff, A1, A2 = sizeof A1 };

/* int, its constants int: character constants, a cast, a negative */
enum chars { C0 = 'a', C1 = (unsigned char) 300, C2 = -'b', C3 = C0 + C2 };

/* long, its constants long: D1 is long while the list is read, so D2 is
   8; D4 makes the type signed, and int does not hold D1 */
enum during {
    D0 = 0x7fffffff,
    D1,
    D2 = sizeof D1,
    D3 = sizeof(D0 + 0),
    D4 = -1
};

/* an untagged enumeration, unsigned int, named by a typedef */
typedef enum { T0, T1, T2 } kind_t;

/* each enumeration and the letter its functions take in their names */
#define ENUMS(X)        \
    X(enum pos, p)      \
    X(enum neg, n)      \
    X(enum big, b)      \
    X(enum bigneg, bn)  \
    X(enum huge, h)     \
    X(enum hugeneg, hn) \
    X(enum after, a)    \
    X(enum chars, c)    \
    X(enum during, d)

/* every constant above */
#define CONSTANTS(X) \
    X(P0)            \
    X(P1)            \
    X(P2)            \
    X(P3)            \
    X(N0)            \
    X(N1)            \
    X(N2)            \
    X(BG0)           \
    X(BG1)           \
    X(BG2)           \
    X(BN0)           \
    X(BN1)           \
    X(H0)            \
    X(H1)            \
    X(H2)            \
    X(HN0)           \
    X(HN1)           \
    X(HN2)           \
    X(A0)            \
    X(A1)            \
    X(A2)            \
    X(C0)            \
    X(C1)            \
    X(C2)            \
    X(C3)            \
    X(D0)            \
    X(D1)            \
    X(D2)            \
    X(D3)            \
    X(D4)
