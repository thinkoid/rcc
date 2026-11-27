/* A value held across && or || and a call.  a + (b && c) holds a in
   eax for the sum, so it is spilled before the paths part and the && is
   computed in ecx, out of the register a is reloaded into, movq
   -24(%rbp), %rax and addl %ecx, %eax.  a + (b && g(c)): the call in
   the right operand pins rax, so the sum names no register to a, and
   nothing is held out of the right operand: the && is computed in eax
   and a is read in its slot, addl -24(%rbp), %eax.  The call inside a
   path has nothing to spill, every value being spilled before the
   paths.  A comparison with a call on either side: a < g(b) reloads a
   into rcx, cmpl %eax, %ecx and setl, g(a) < b compares eax with b in
   its slot. */
int g(int a);

int
and_no_call(int a, int b, int c)
{
        return a + (b && c);
}

int
and_call(int a, int b, int c)
{
        return a + (b && g(c));
}

int
less_than_call(int a, int b)
{
        return a < g(b);
}

int
call_less_than(int a, int b)
{
        return g(a) < b;
}

int
equal_call(int a, int b)
{
        return a == g(b);
}
