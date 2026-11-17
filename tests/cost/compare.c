/* backend.md 6.3 and aarch64.md 6.3, a < b and the rest as a value: a
   compare, and its condition written into the register as 1 or 0.
   backend.md 6.3, the width rule: the value 0 or 1, of a comparison,
   of ! and of &&, is valid to 64 bits, signed or unsigned, widened to
   a long and an unsigned long and assigned to a register long.

   On x86-64 the listing must show cmpl -8(%rbp), %eax, setl %al and
   movzbl %al, %eax in less(), the same with sete in equal(), and no
   conditional jump.  In less_long(), less_unsigned_long(), not_long()
   and both_long(), nothing after movzbl %al, %eax or after the arms'
   movl $1, %eax and movl $0, %eax: no movslq and no movl %eax, %eax.
   In less_register(), the comparison written into the register, setl
   %bl; movzbl %bl, %ebx; in not_register() and both_register(), the
   value computed in eax and moved, movq %rax, %rbx, with no movslq
   before the move.

   On aarch64 the listing must show cmp w0, w1 and cset w0, lt in
   less(), the same with cset w0, eq in equal(), and no conditional
   branch; in the widened functions, cset w0, lt, cset w0, eq, mov w0,
   #1 and mov w0, #0 followed by sxtw x0, w0, and sxtw x19, w0 into
   the register long. */
int less(int a, int b)
{
    return a < b;
}

int equal(int a, int b)
{
    return a == b;
}

long
less_long(int a, int b)
{
        return (long) (a < b);
}

unsigned long
less_unsigned_long(int a, int b)
{
        return (unsigned long) (a < b);
}

long
not_long(int a)
{
        return (long) !a;
}

long
both_long(int a, int b)
{
        return (long) (a && b);
}

long
less_register(int a, int b)
{
        register long l = a < b;

        return l;
}

long
not_register(int a)
{
        register long l = !a;

        return l;
}

long
both_register(int a, int b)
{
        register long l = a && b;

        return l;
}
