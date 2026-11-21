/* The list the generic probe is generated from.  generic.c makes a
   function of each selection, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_; generic-main.c calls both and
   compares the number each selects.  The objects the selections name
   are declared here and defined nowhere: a controlling expression is
   not evaluated, 6.5.2.1p3, and no selected expression names one.

   A selection chooses by C's compatibility, docs/shape.md chapters 3
   and 5, of the controlling expression's type, decayed and of no
   qualifier at its top: by an integer type's kind, an enumeration's
   identity, a pointee's qualifiers, and an association's qualifiers at
   its top, which select nothing.  No selection turns on restrict,
   which rcc drops (shape.md chapter 3), where C keeps it. */

/* unsigned int, int and unsigned long, as clang chooses */
enum pos { P0, P1 };
enum neg { N0 = -1 };
enum wide { W0 = 0x100000000 };

struct pair
{
        int a, b;
};

extern char c;
extern signed char sc;
extern unsigned char uc;
extern short s;
extern int i;
extern const int ci;
extern long l;
extern long *lp;
extern enum pos pos;
extern enum neg neg;
extern enum wide wide;
extern char *p;
extern char **pp;
extern const char *cp;
extern const char *const *cpp;
extern const int *cip;
extern volatile int *vip;
extern char a[4];
extern const char ca[4];
extern const struct pair pairs[2];
extern int (*rows)[3];
extern __WCHAR_TYPE__ *wp;
int count(void);

/* each selection: its name, its controlling expression and its
   associations */
/* clang-format off */
#define SELECTIONS(X)                                                   \
    X(plain_char, c, char: 1, signed char: 2, unsigned char: 3)         \
    X(signed_char, sc, char: 1, signed char: 2, unsigned char: 3)       \
    X(unsigned_char, uc, char: 1, signed char: 2, unsigned char: 3)     \
    X(character, 'a', char: 1, int: 2)                                  \
    X(promoted, +c, char: 1, int: 2)                                    \
    X(shorts, s, short: 1, unsigned short: 2, int: 3)                   \
    X(longs, 1L, long: 1, long long: 2)                                 \
    X(long_longs, 1LL, long: 1, long long: 2)                           \
    X(unsigned_long_longs, 1ULL, unsigned long: 1,                      \
      unsigned long long: 2)                                            \
    X(long_sum, 1L + 1LL, long: 1, long long: 2)                        \
    X(unsigned_sum, 1UL + 1LL, unsigned long: 1, long long: 2,          \
      unsigned long long: 3)                                            \
    X(mixed_sum, 1U + 1L, unsigned: 1, long: 2, unsigned long: 3)       \
    X(sizes, sizeof 0, unsigned long: 1, unsigned long long: 2)         \
    X(difference, p - p, long: 1, long long: 2)                         \
    X(comparison, c < s, int: 1, default: 0)                            \
    X(shift, c << 1L, int: 1, long: 2)                                  \
    X(arms, i ? 1 : 2L, int: 1, long: 2)                                \
    X(constant, P0, int: 1, unsigned: 2)                                \
    X(own_enumeration, pos, enum pos: 1, enum neg: 2, default: 0)       \
    X(other_enumeration, pos, enum neg: 1, default: 2)                  \
    X(enumeration_unsigned, pos, unsigned: 1, int: 2)                   \
    X(enumeration_int, neg, unsigned: 1, int: 2)                        \
    X(enumeration_long, wide, unsigned long: 1, long: 2)                \
    X(long_pointer, lp, long *: 1, long long *: 2)                      \
    X(const_pointee, cp, char *: 1, const char *: 2)                    \
    X(plain_pointee, p, char *: 1, const char *: 2)                     \
    X(volatile_pointee, vip, int *: 1, volatile int *: 2,               \
      const volatile int *: 3)                                          \
    X(joined_pointees, i ? cip : vip, const volatile int *: 1,          \
      default: 0)                                                       \
    X(pointer_pointee, pp, char **: 1, const char **: 2,                \
      char *const *: 3)                                                 \
    X(const_pointer_pointee, cpp, const char **: 1,                     \
      const char *const *: 2)                                           \
    X(array, a, char *: 1, const char *: 2)                             \
    X(const_array, ca, char *: 1, const char *: 2)                      \
    X(array_address, &a, char (*)[4]: 1, char *: 2)                     \
    X(composite_row, rows, int (*)[]: 1, default: 0)                    \
    X(qualified_association, i, const int: 1, int: 2)                   \
    X(qualified_object, ci, int: 1, default: 0)                         \
    X(designator, count, int (*)(void): 1, default: 0)                  \
    X(record, pairs[0], struct pair: 1, default: 0)                     \
    X(string, "x", char *: 1, const char *: 2)                          \
    X(wide_string, L"x", __WCHAR_TYPE__ *: 1, default: 0)               \
    X(nested, _Generic(i, int: 1L, default: 0), long: 1, default: 0)    \
    X(glibc_pointer, 0 ? (p) : (void *) 1, const void *: 1, default: 2) \
    X(glibc_array, 0 ? (a) : (void *) 1, const void *: 1, default: 2)   \
    X(glibc_wide, 0 ? (wp) : (void *) 1, const void *: 1, default: 2)   \
    X(glibc_const_pointer, 0 ? (cp) : (void *) 1, const void *: 1,      \
      default: 2)                                                       \
    X(glibc_const_array, 0 ? (ca) : (void *) 1, const void *: 1,        \
      default: 2)                                                       \
    X(glibc_const_records, 0 ? (pairs) : (void *) 1, const void *: 1,   \
      default: 2)                                                       \
    X(glibc_const_pointer_pointee, 0 ? (cpp) : (void *) 1,              \
      const void *: 1, default: 2)
/* clang-format on */
