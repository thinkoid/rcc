/* backend.md 7.1 and aarch64.md 7.1, a condition whose operator's own
   instruction sets the zero flag, after the Seventh Edition compiler's
   cctab.  add, sub, or, xor and neg test +, -, |, ^ and unary -;
   test_register, test_immediate and test_variable test & of two
   parameters, of an immediate and of a register variable; each compound
   assignment that has a memory form, and each prefix step, is tested on
   a local, a char local, a global and a register variable, in
   <op>_local, <op>_char, <op>_global and <op>_register; equal_zero and
   unequal_zero compare such a condition with 0 by == and !=, and
   zero_equal and zero_unequal with the 0 on the left, which is taken as
   the 0 on the right (backend.md 6.1).  Beside
   them, product, shift, postfix, narrow_register and negative keep the
   cmp: a product, a shift by a variable count, a postfix step, whose
   value is the old one, the step of a register char, re-extended after
   it, and < 0.

   On x86-64 the listing must show the jump on the flags of the
   operator's own instruction, with no cmp: addl -8(%rbp), %eax, b's
   slot the instruction's operand, then je .Ladd.2, and the same after
   subl, orl, xorl and negl %eax; testl -8(%rbp), %eax,
   testl $1, -4(%rbp) on a in its slot, and testl -16(%rbp), %ebx, the
   register variable read in ebx and not copied, each then je; the
   memory form then je, addl %eax, -12(%rbp) on the local,
   addb %al, -9(%rbp) on the char, addl %eax, counter(%rip) on the
   global, and the operation in place on the register variable, b's
   slot its operand, addl -16(%rbp), %ebx; the steps
   addl $1 and subl $1, addb $1 and subb $1 on the char; in equal_zero
   testl -8(%rbp), %eax then jne .Lequal_zero.2, and in unequal_zero
   subl %eax, -12(%rbp) then je .Lunequal_zero.2; in zero_equal
   testl $1, -4(%rbp) then jne .Lzero_equal.2, and in zero_unequal the
   same then je, with no load of the 0 and no cmp.  It must show imull,
   shll %cl, the old value copied in postfix, movsbl %bl, %ebx after
   the addl in narrow_register and the addl of negative each followed
   by a cmp at the condition's width, cmpl $0, %ecx in postfix and
   cmpb $0, %bl in narrow_register, and the jump, jge .Lnegative.2 in
   negative.

   On aarch64, whose test of zero is cbz or cbnz on a register and
   reads no flag, the listing must show each condition's value and then
   cbz: add w0, w0, w1 then cbz w0, .Ladd.2; and w0, w0, #0x1 then cbz
   in test_immediate; on an object the load, the operation and the
   store, str w1 then cbz w1, since the machine has no memory form; on
   a char, whose register holds stale upper bits, tst w1, #0xff and
   b.eq; in place on a register variable, add w19, w19, w0 then
   cbz w19, and in narrow_register uxtb w19, w19 between the add and
   the cbz; and
   equal_zero, unequal_zero and negative as comparisons, cmp w0, #0
   and b.ne, b.eq and b.ge; zero_equal and zero_unequal the same,
   and w0, w0, #0x1 then cmp w0, #0 and b.ne or b.eq, with no load of
   the 0. */
int counter;

int
add(int a, int b)
{
        if (a + b)
                return 1;
        return 0;
}

int
sub(int a, int b)
{
        if (a - b)
                return 1;
        return 0;
}

int
or(int a, int b)
{
        if (a | b)
                return 1;
        return 0;
}

int
xor(int a, int b)
{
        if (a ^ b)
                return 1;
        return 0;
}

int
neg(int a)
{
        if (-a)
                return 1;
        return 0;
}

int
test_register(int a, int b)
{
        if (a & b)
                return 1;
        return 0;
}

int
test_immediate(int a)
{
        if (a & 1)
                return 1;
        return 0;
}

int
test_variable(int a, int b)
{
        register int x = a;

        if (x & b)
                return 1;
        return 0;
}

int
add_to_local(int a, int b)
{
        int x = a;

        if (x += b)
                return 1;
        return 0;
}

int
add_to_char(int a, int b)
{
        char x = a;

        if (x += b)
                return 1;
        return 0;
}

int
add_to_global(int b)
{
        if (counter += b)
                return 1;
        return 0;
}

int
add_to_register(int a, int b)
{
        register int x = a;

        if (x += b)
                return 1;
        return 0;
}

int
sub_from_local(int a, int b)
{
        int x = a;

        if (x -= b)
                return 1;
        return 0;
}

int
sub_from_char(int a, int b)
{
        char x = a;

        if (x -= b)
                return 1;
        return 0;
}

int
sub_from_global(int b)
{
        if (counter -= b)
                return 1;
        return 0;
}

int
sub_from_register(int a, int b)
{
        register int x = a;

        if (x -= b)
                return 1;
        return 0;
}

int
and_to_local(int a, int b)
{
        int x = a;

        if (x &= b)
                return 1;
        return 0;
}

int
and_to_char(int a, int b)
{
        char x = a;

        if (x &= b)
                return 1;
        return 0;
}

int
and_to_global(int b)
{
        if (counter &= b)
                return 1;
        return 0;
}

int
and_to_register(int a, int b)
{
        register int x = a;

        if (x &= b)
                return 1;
        return 0;
}

int
or_to_local(int a, int b)
{
        int x = a;

        if (x |= b)
                return 1;
        return 0;
}

int
or_to_char(int a, int b)
{
        char x = a;

        if (x |= b)
                return 1;
        return 0;
}

int
or_to_global(int b)
{
        if (counter |= b)
                return 1;
        return 0;
}

int
or_to_register(int a, int b)
{
        register int x = a;

        if (x |= b)
                return 1;
        return 0;
}

int
xor_to_local(int a, int b)
{
        int x = a;

        if (x ^= b)
                return 1;
        return 0;
}

int
xor_to_char(int a, int b)
{
        char x = a;

        if (x ^= b)
                return 1;
        return 0;
}

int
xor_to_global(int b)
{
        if (counter ^= b)
                return 1;
        return 0;
}

int
xor_to_register(int a, int b)
{
        register int x = a;

        if (x ^= b)
                return 1;
        return 0;
}

int
inc_local(int a)
{
        int x = a;

        if (++x)
                return 1;
        return 0;
}

int
inc_char(int a)
{
        char x = a;

        if (++x)
                return 1;
        return 0;
}

int
inc_global(void)
{
        if (++counter)
                return 1;
        return 0;
}

int
inc_register(int a)
{
        register int x = a;

        if (++x)
                return 1;
        return 0;
}

int
dec_local(int a)
{
        int x = a;

        if (--x)
                return 1;
        return 0;
}

int
dec_char(int a)
{
        char x = a;

        if (--x)
                return 1;
        return 0;
}

int
dec_global(void)
{
        if (--counter)
                return 1;
        return 0;
}

int
dec_register(int a)
{
        register int x = a;

        if (--x)
                return 1;
        return 0;
}

int
equal_zero(int a, int m)
{
        if ((a & m) == 0)
                return 1;
        return 0;
}

int
unequal_zero(int a, int b)
{
        int x = a;

        if ((x -= b) != 0)
                return 1;
        return 0;
}

int
zero_equal(int c)
{
        if (0 == (c & 1))
                return 1;
        return 0;
}

int
zero_unequal(int c)
{
        if (0 != (c & 1))
                return 1;
        return 0;
}

int
product(int a, int b)
{
        if (a * b)
                return 1;
        return 0;
}

int
shift(int a, int n)
{
        if (a << n)
                return 1;
        return 0;
}

int
postfix(int a)
{
        int x = a;

        if (x++)
                return 1;
        return 0;
}

int
narrow_register(int a)
{
        register char x = a;

        if (++x)
                return 1;
        return 0;
}

int
negative(int a, int b)
{
        if ((a + b) < 0)
                return 1;
        return 0;
}
