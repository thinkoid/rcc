/* Unary +, 6.5.4.3p2: the operand's value in its promoted type.  The
   source writes no operation for the +, so the tree holds the
   promotion alone: the cast of a narrow operand, which c1 writes as
   its extending load, and nothing for an int or a wider one.  Each
   function compiles to its twin with the cast to the promoted type in
   place of the +, (int) c for +c.

   On x86-64 the listing must show signed_char() and plain_char()
   reading c by its extending load, movsbl -1(%rbp), %eax, and nothing
   after it; unsigned_short() by movzwl -2(%rbp), %eax; int_() and
   long_() by the load alone, movl -4(%rbp), %eax and
   movq -8(%rbp), %rax; register_() by the move of its register,
   movl %ebx, %eax, as for r itself; and operand() adding +b to a as
   it adds b, movsbl -5(%rbp), %ecx; addl %ecx, %eax.

   On aarch64 the listing must show signed_char() reading c by
   ldrsb w0, [x29, #16] and plain_char() by ldrb w0, [x29, #16], plain
   char being unsigned there, and nothing after either;
   unsigned_short() by ldrh w0, [x29, #16]; int_() and long_() by the
   load alone, ldr w0, [x29, #16] and ldr x0, [x29, #16]; register_()
   by the move of its register, mov w0, w19, as for r itself; and
   operand() adding +b to a as it adds b, ldrsb w1, [x29, #20];
   add w0, w0, w1. */
int
signed_char(signed char c)
{
        return +c;
}

int
plain_char(char c)
{
        return +c;
}

int
unsigned_short(unsigned short s)
{
        return +s;
}

int
int_(int i)
{
        return +i;
}

long
long_(long l)
{
        return +l;
}

int
register_(register int r)
{
        return +r;
}

int
operand(int a, signed char b)
{
        return a + +b;
}
