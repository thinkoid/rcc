/* The lists the blocks probe is generated from.  blocks.c defines each
   function of the list, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  blocks-lib.c holds the functions
   they call, compiled by clang alone and linked under both names.
   blocks-main.c calls both builds over a table of values and compares
   the results and the record of the calls each build made.  GNU C's
   statement expression, docs/shape.md chapter 5 and docs/backend.md
   6.7: its statements where it stands, values held beside it saved as
   at a branch where the statements may move them and left in their
   registers beside straight-line statements, its value in each
   context, a jump out of it, and a label in it with a goto to it from
   inside. */

/* clang-format off */
#define FUNCTIONS(X)                                                    \
    X(used) X(held) X(nested) X(broken) X(continued) X(returned)        \
    X(looped) X(siblings) X(arguments) X(rewritten) X(switched)         \
    X(counted) X(left) X(tested) X(narrow) X(effect) X(into_register)   \
    X(compound) X(placed) X(shifted) X(divided) X(arms) X(pressure)     \
    X(logical) X(pointer) X(labeled) X(escaped) X(beside) X(local)      \
    X(straight)
/* clang-format on */

#define DECLARE(name)          \
        long name(long, long); \
        long ref_##name(long, long);

FUNCTIONS(DECLARE)

/* blocks-lib.c's: the record of the calls, a call that adds its
   argument to it and returns it, and a function of ten parameters */
extern long trace;
extern long ref_trace;

long mark(long);
long ten(long, long, long, long, long, long, long, long, long, long);
