/* The lists the bools probe is generated from.  bools.c makes functions
   of each entry, compiled by rcc and, as the reference, by clang -O0
   under the prefix ref_; bools-main.c calls both over a table of values
   and compares.  Every _Bool the functions read holds 0 or 1: a byte of
   another value is G21's, which docs/guarantees.md gives and clang does
   not, and tests/corpus/g21-bool-byte.c pins it, not this probe.

   A conversion to _Bool is the parser's tree (_Bool) (E != 0), a value
   already 0 or 1 narrowed alone, and each operator that stores into a
   _Bool stores the conversion of its result, docs/shape.md chapter 5. */

/* the types converted to _Bool, and the letters each takes in a
   function's name */
#define CONVERSIONS(X)       \
        X(int, i)            \
        X(unsigned, u)       \
        X(long, l)           \
        X(unsigned long, ul) \
        X(signed char, sc)   \
        X(unsigned short, us)

/* the compound assignments of any right operand, of a nonzero one and
   of a count, each with its name */
#define COMPOUNDS(X) \
        X(+=, add)   \
        X(-=, sub)   \
        X(*=, mul)   \
        X(&=, and)   \
        X(|=, or)    \
        X(^=, xor)
#define DIVIDING(X) \
        X(/=, div)  \
        X(%=, rem)
#define SHIFTS(X)   \
        X(<<=, shl) \
        X(>>=, shr)

/* the four steps, each with its name */
#define STEPS(X)         \
        X(pre_inc, ++b)  \
        X(pre_dec, --b)  \
        X(post_inc, b++) \
        X(post_dec, b--)

/* the library's functions, compiled by clang alone, which rcc's code
   calls with a _Bool argument and whose _Bool results it reads */
_Bool lib_negate(int x);
_Bool lib_identity(_Bool b);
