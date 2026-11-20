/* The lists the members probe is generated from.  members.c makes the
   functions of each member of each record of the layout probe,
   compiled by rcc and, as the reference, by clang -O0 under the prefix
   ref_; members-main.c calls both and compares.  A member access is
   the dereference of the record's address plus the member's offset,
   docs/shape.md chapter 4: each member is read, written, stepped,
   added to and its address taken, through ->, through . on an element
   of an array of records, written and added to there across a call,
   and on a local and a global of its record.
   The records are layout.h's. */

#include "layout.h"

/* for each member kind, by its letter: the path from the member to a
   scalar in it, the scalar's type, and the scalar stepped and added 3
   to, as a long, or 0 for a function pointer, which takes neither */
#define PATH_c
#define PATH_s
#define PATH_i
#define PATH_l
#define PATH_p
#define PATH_f
#define PATH_a [1]
#define PATH_h [2]
#define PATH_r .l
#define PATH_u .i
#define PATH_n [1].c

#define SCALAR_c char
#define SCALAR_s short
#define SCALAR_i int
#define SCALAR_l long
#define SCALAR_p pointer_t
#define SCALAR_f function_t
#define SCALAR_a char
#define SCALAR_h short
#define SCALAR_r long
#define SCALAR_u int
#define SCALAR_n char

#define STEP(E)   ((long) (E)++)
#define STEP_c    STEP
#define STEP_s    STEP
#define STEP_i    STEP
#define STEP_l    STEP
#define STEP_p    STEP
#define STEP_f(E) 0L
#define STEP_a    STEP
#define STEP_h    STEP
#define STEP_r    STEP
#define STEP_u    STEP
#define STEP_n    STEP

#define ADD(E)   ((long) ((E) += 3))
#define ADD_c    ADD
#define ADD_s    ADD
#define ADD_i    ADD
#define ADD_l    ADD
#define ADD_p    ADD
#define ADD_f(E) 0L
#define ADD_a    ADD
#define ADD_h    ADD
#define ADD_r    ADD
#define ADD_u    ADD
#define ADD_n    ADD

/* the scalar added to across a call, held(): the address of p[i]'s
   member is held across it */
long held(long);
#define HELD_ADD(E)   ((long) ((E) += held(3)))
#define HELD_ADD_c    HELD_ADD
#define HELD_ADD_s    HELD_ADD
#define HELD_ADD_i    HELD_ADD
#define HELD_ADD_l    HELD_ADD
#define HELD_ADD_p    HELD_ADD
#define HELD_ADD_f(E) 0L
#define HELD_ADD_a    HELD_ADD
#define HELD_ADD_h    HELD_ADD
#define HELD_ADD_r    HELD_ADD
#define HELD_ADD_u    HELD_ADD
#define HELD_ADD_n    HELD_ADD

/* a list a register pointer walks */
struct link
{
        long value;
        struct link *next;
};

/* a struct of a function pointer, called through the member */
struct ops
{
        int bias;
        int (*twice)(int);
};
