/* backend.md 6.3 and aarch64.md 6.3, p - q: the subtraction, then a
   division by the element size, not a shift, for a size of 4 (G6).
   backend.md 6.5, p - q with a register variable q on the left: the
   right operand is generated outside rax and rdx, as a divisor is, at
   a size of 4 and 12, with the variable's read and with an
   assignment's value; at a size of 1 nothing is reserved.  With a call
   on the right, called() returns p - h(q), whose quotient stays in
   rax, which the return names; beside it called_count() takes the
   quotient as a shift's count, which names rcx, and called_char()
   returns p - hc(q) on two char *, which divides by nothing.

   On x86-64 the listing must show, in diff(), subq -16(%rbp), %rax of
   the two addresses, q's slot the instruction's operand (backend.md
   6.1), the size in a register with movq $4, %rcx, since idiv takes
   no immediate, cqto and idivq %rcx.  In register_int(),
   register_row() and assigned(), the array's address in rcx, leaq
   ints(%rip), %rcx, the copy of q into rax, movq %rbx, %rax, subq
   %rcx, %rax, the size in rcx, movq $4, %rcx or movq $12, %rcx, cqto,
   idivq %rcx and no move after; assigned() first writes q, leaq
   ints+4(%rip), %rbx.  In register_char(), the address in rcx, leaq
   chars(%rip), %rcx, outside rax, which the return names (backend.md
   6.1), the copy in rax, movq %rbx, %rax, subq %rcx, %rax and no move
   after.  In called() the call, subq %rax, %rcx, the size in rsi,
   movq %rcx, %rax, cqto and idivq %rsi, with no move after it; in
   called_count() the quotient moved into rcx for the count, movq %rax,
   %rcx, and shlq %cl, %rax; and in called_char() subq %rax, %rcx and
   the return's movq %rcx, %rax.

   On aarch64 the listing must show sub x0, x0, x1 of the two
   addresses, the size in a register with mov x1, #4, since sdiv takes
   no immediate, and sdiv x0, x0, x1; with the register variable, sub
   x0, x19, x0 after the array's address in x0, adrp and add, then mov
   x1, #4 or mov x1, #12 and sdiv x0, x0, x1 but in register_char();
   in assigned(), the address of ints+4 in the register, adrp x19,
   ints+4 and add x19, x19, :lo12:ints+4, and sub x0, x19, x0 after
   the array's address in x0.  In called() and called_count() sub x1,
   x1, x0 after the call, mov x0, #4 and sdiv x0, x1, x0 and sdiv x1,
   x1, x0, the latter's count read by lsl x0, x0, x1; in called_char()
   sub x0, x1, x0. */
long diff(int *p, int *q)
{
    return p - q;
}

char chars[8];
int ints[8];
int rows[8][3];

long
register_char(char *p)
{
        register char *q = p;

        return q - chars;
}

long
register_int(int *p)
{
        register int *q = p;

        return q - ints;
}

long
register_row(int (*p)[3])
{
        register int (*q)[3] = p;

        return q - rows;
}

long
assigned(void)
{
        register int *q;

        return (q = &ints[1]) - ints;
}

int *h(int *);
char *hc(char *);

long
called(int *p, int *q)
{
        return p - h(q);
}

long
called_count(long x, int *p, int *q)
{
        return x << (p - h(q));
}

long
called_char(char *p, char *q)
{
        return p - hc(q);
}
