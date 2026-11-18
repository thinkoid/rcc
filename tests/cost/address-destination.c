/* backend.md chapter 4 and 6.3, an address assigned to a register
   variable: a pointer plus a constant index is the leaq of the
   pointer's value and the index times the size, and on x86-64 the
   variable's register is the destination of both, the pointer's own
   row writing it and the leaq after it, so no expression register is
   taken.  Each function's variable is q, in the first register
   variable's register, and in the functions on a register p the
   second.

   row() and element() write q = p[1] on a pointer to rows of three
   ints and q = &p[1] on an int *; row_register() and
   element_register() the same with p a register variable, whose read
   is the leaq's base already.  declared() initializes the variable,
   register int *q = p[1], and through() takes the pointer through
   another, q = &(*pp)[2].  itself() writes q = &q[1], whose pointer
   is the variable's own read, and called() q = &f(q)[1], whose
   pointer is a call's result, which the call's row leaves in rax.
   global() writes q = &xa[3] on an array the unit only declares,
   whose address the GOT gives: an lvalue's address plus a constant,
   which is not a pointer's.

   On x86-64 the listing must show row() as movq -16(%rbp), %rbx and
   leaq 12(%rbx), %rbx, and element() the same with leaq 4(%rbx),
   %rbx; row_register() and element_register() as the one leaq
   12(%rbx), %r12 and leaq 4(%rbx), %r12; declared() as row();
   through() as movq (%rax), %rbx and leaq 8(%rbx), %rbx after pp's
   load; itself() as the one leaq 4(%rbx), %rbx; called() as leaq
   4(%rax), %rbx after the call; and global() as movq
   xa@GOTPCREL(%rip), %rax and leaq 12(%rax), %rbx.  Before the
   return's movq into rax, no function names an expression register
   but called(), global() and through(), whose pp is loaded into rax.

   On aarch64 the listing must show row() as ldr x0, [x29, #24] and
   add x19, x0, #12, the pointer loaded elsewhere, element() and
   declared() the same with #4 and #12, row_register() and
   element_register() as the one add x20, x19, #12 and add x20, x19,
   #4, and itself() as the one add x19, x19, #4. */
int *f(int *);
extern int xa[];

int *
row(int (*p)[3])
{
        register int *q;

        q = p[1];
        return q;
}

int *
element(int *p)
{
        register int *q;

        q = &p[1];
        return q;
}

int *
row_register(int (*s)[3])
{
        register int (*p)[3] = s;
        register int *q;

        q = p[1];
        return q;
}

int *
element_register(int *s)
{
        register int *p = s;
        register int *q;

        q = &p[1];
        return q;
}

int *
declared(int (*p)[3])
{
        register int *q = p[1];

        return q;
}

int *
through(int **pp)
{
        register int *q;

        q = &(*pp)[2];
        return q;
}

int *
itself(int *p)
{
        register int *q = p;

        q = &q[1];
        return q;
}

int *
called(int *p)
{
        register int *q = p;

        q = &f(q)[1];
        return q;
}

int *
global(void)
{
        register int *q;

        q = &xa[3];
        return q;
}
