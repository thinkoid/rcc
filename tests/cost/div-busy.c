/* backend.md 6.5 and aarch64.md 6.5, the pinned instructions: a
   division wants its dividend in rax and writes rdx, and a node whose
   right operand's top operator is a division generates its left
   operand outside the two, so that nothing it holds is in the way.  In
   a + b / c the + holds a while the division is generated.  The rule
   is a reservation and not a pin, and two functions show the moves
   it leaves: in div_deeper's a + (b / c + 1) the division is deeper in
   the right operand, and the rule does not reach it; in div_held the +
   over u / v keeps a to g out of rax and rdx, they take the seven
   other registers, and x, whose own reservation cannot be met, takes
   rax.  A remainder's value is the value in rdx, where the division
   leaves it (backend.md 6.5), and the rem_ functions use it: stored,
   rem_stored; returned, rem_returned; as the left operand of a next
   row, rem_left, and as the right one, rem_busy; as an argument,
   rem_argument; as the dividend of a quotient, rem_quotient, and of a
   remainder, rem_remainder; as the result of %= on a register
   variable, rem_register, and on a local, rem_local; and as the first
   arm of ?:, rem_arm, whose arms join in the register the return names
   (backend.md 6.1).  A quotient stays in rax where the consumer names
   it, though the name was withheld from the dividend for a division
   or a call within the right operand: quotient_nested returns x / (y
   / z), and quotient_compound x /= f(y); beside them remainder_called
   returns x % f(y), whose remainder is in rdx.

   On x86-64, where c is the division's operand in its slot (backend.md
   6.1), the listing must show, in div_busy and rem_busy, a loaded into
   ecx and b into eax, cltd, idivl -12(%rbp), then in div_busy addl
   %ecx, %eax, which writes the quotient's register, the one the return
   names, and no move, and in rem_busy addl %edx, %ecx and the return's
   movl %ecx, %eax, since the remainder is in rdx, which the return
   does not name.
   div_deeper must show a in eax and b in ecx, a moved out of the
   pinned rax, movq %rax, %rsi, b moved into eax, cltd, idivl
   -12(%rbp), the quotient moved into b's register, movl %eax, %ecx,
   and the adds.  div_held must show a to g in ecx, esi, edi and r8d to
   r11d, x in eax and y in edx, then, no register being free, a spilled,
   movq %rcx, -32(%rbp), x moved into a's register, movq %rax, %rcx, y
   moved into eax, which is then y's register, since the division
   overwrites edx, cltd, idivl 40(%rbp), the quotient added where idiv
   left it, addl %eax, %ecx, a reloaded for its add,
   movq -32(%rbp), %rcx, and the last quotient's register written by
   the sum, addl %ecx, %eax, with no move.

   After each rem_ function's idivl the listing must show the remainder
   taken from edx and no other move: movl %edx, (%rcx) in rem_stored;
   the return's movl %edx, %eax in rem_returned; addl -4(%rbp), %edx
   and the return's move in rem_left; the argument's store,
   movl %edx, -12(%rbp), in rem_argument; in rem_quotient and
   rem_remainder the second division's dividend moved into eax,
   movl %edx, %eax, before cltd, and in rem_remainder the return's
   movl %edx, %eax after the second idivl; movl %edx, %ebx in
   rem_register and movl %edx, -12(%rbp) in rem_local.  rem_arm must
   show the first arm's idivl and the remainder moved into the
   register the return names, movl %edx, %eax, before its jump, the
   second arm's load into it, movl -12(%rbp), %eax, and no move after
   the join: 16 instructions.  quotient_nested must show the inner
   idivl -12(%rbp), x in ecx moved into eax and idivl %esi, with no
   move after it; quotient_compound the call, x in ecx moved into eax,
   idivl %esi and the store movl %eax, -4(%rbp), with no move after
   it; and remainder_called idivl %ecx and the return's movl %edx,
   %eax.

   On aarch64 the listing is the division on the registers the loads
   took, a in w0, b in w1 and c in w2: sdiv w1, w1, w2 in div_busy(),
   sdiv w3, w1, w2 and msub w1, w3, w2, w1 in rem_busy(), then
   add w0, w0, w1, and no move, since no instruction pins a register;
   the row is not reached there.  div_deeper is sdiv w1, w1, w2,
   add w1, w1, #1 and add w0, w0, w1; div_held loads a to x into w0 to
   w7, y and z into w9 and w10, and divides, sdiv w9, w9, w10, with no
   move and no spill.  The rem_ functions are sdiv and msub on the
   registers the loads took, msub writing the register the remainder's
   use reads, w0 for a return, and rem_arm joins its arms in w0 with no
   move.  quotient_nested is sdiv w1, w1, w2 and sdiv w0, w0, w1,
   quotient_compound sdiv w0, w1, w0 after the call and the store from
   w0, and remainder_called sdiv w2, w1, w0 and msub w0, w2, w0, w1. */
int div_busy(int a, int b, int c)
{
    return a + b / c;
}

int rem_busy(int a, int b, int c)
{
    return a + b % c;
}

int div_deeper(int a, int b, int c)
{
    return a + (b / c + 1);
}

int div_held(int a, int b, int c, int d, int e, int f, int g, int x, int y,
             int z, int u, int v)
{
    return (a + (b + (c + (d + (e + (f + (g + (x + y / z)))))))) + u / v;
}

int f(int);

void
rem_stored(int *p, int x, int y)
{
        *p = x % y;
}

int
rem_returned(int x, int y)
{
        return x % y;
}

int
rem_left(int a, int b, int c)
{
        return b % c + a;
}

int
rem_argument(int x, int y)
{
        return f(x % y);
}

int
rem_quotient(int x, int y, int z)
{
        return x % y / z;
}

int
rem_remainder(int x, int y, int z)
{
        return x % y % z;
}

int
rem_register(int x, int y)
{
        register int r = x;

        r %= y;
        return r;
}

int
rem_local(int x, int y)
{
        int l = x;

        l %= y;
        return l;
}

int
rem_arm(int c, int x, int y)
{
        return c ? x % y : y;
}

int
quotient_nested(int x, int y, int z)
{
        return x / (y / z);
}

int
quotient_compound(int x, int y)
{
        return x /= f(y);
}

int
remainder_called(int x, int y)
{
        return x % f(y);
}
