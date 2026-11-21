/* The lists the stack probe is generated from.  stack.c defines each
   function of the list, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_.  stack-lib.c holds the functions
   they call, compiled by clang alone and linked under both names.
   stack-main.c calls both builds over a table of values and compares
   the results.  A variable length array's storage, docs/shape.md
   chapter 5, docs/backend.md chapter 5 and docs/aarch64.md 5.2, G23:
   its elements written by rcc's code and read by clang's, sizeof after
   the size's operand changes, a call with arguments on the stack and
   the stack pointer's alignment at a call while the storage is live;
   and the storage given back where the array's scope is left, so that
   the next array takes the same address: at a block's end each time
   around a loop of many turns, by a break, a continue and a goto out
   of the scope, by a goto back before the declaration, at the end of a
   statement expression in each context and of a for that declares one;
   two arrays of one block, and the first kept while a goto back between
   them gives back the second's; an array in a nested block.  An
   address is compared only with another of the same build. */

/* clang-format off */
#define FUNCTIONS(X)                                                    \
    X(element) X(sized) X(stacked) X(aligned) X(turns) X(broken)        \
    X(continued) X(left) X(back) X(valued) X(tested) X(effect)          \
    X(declared) X(two) X(between) X(nested)
/* clang-format on */

#define DECLARE(name)          \
        long name(long, long); \
        long ref_##name(long, long);

FUNCTIONS(DECLARE)

/* stack-lib.c's: a sum of n longs, a function of ten parameters, the
   misalignment of the frame of a call, and a call whose frame writes
   the stack below its caller's stack pointer */
long sum(const long *, long);
long ten(long, long, long, long, long, long, long, long, long, long);
long frame_misalignment(void);
long scribble(long);
