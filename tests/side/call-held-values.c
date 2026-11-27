/* After the call, the consumer takes the spilled value.  On x86-64 a
   spilled left operand of + * & | ^ is read in its slot, addl
   -24(%rbp), %eax, where the result register is named or no free one
   is; - reloads it outside the call's register, movq -24(%rbp), %rcx,
   and moves the difference to eax for a return; / moves the divisor
   out of rax and reloads the dividend into rax; << reloads the value
   outside the count's register and rcx and moves the count to ecx.  On
   aarch64 every operator reloads the left operand into the next free
   register, ldr x1, [x29, #32], and writes the register the consumer
   names, add w0, w1, w0, or the left's own where none is named.  A
   store, an initializer, an argument and a discarded value name no
   register; an enclosing node reads its own operand after. */
int g(int a);
int k(int a, int b);

int
sum(int a, int b)
{
        return a + g(b);
}

int
product(int a, int b)
{
        return a * g(b);
}

int
and(int a, int b)
{
        return a & g(b);
}

int
difference(int a, int b)
{
        return a - g(b);
}

int
quotient(int a, int b)
{
        return a / g(b);
}

int
modulo(int a, int b)
{
        return a % g(b);
}

int
shifted(int a, int b)
{
        return a << g(b);
}

int
stores(int a, int b, int c)
{
        c = a + g(b);
        c = a - g(b);
        int x = a + g(b);
        a + g(b);
        return k(a + g(b), c) + x;
}

int
enclosing(int a, int b, int c)
{
        return a + g(b) + c;
}

int
under(int a, int b, int c)
{
        return a + g(b) * c;
}
