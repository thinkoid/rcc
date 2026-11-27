/* && and || as values with values held, 6.5.14p3, 6.5.15p3: before
   the paths part, every value held in a register is spilled, in the
   order of the registers, so that both paths find it where the other
   left it, and the operator's value is taken in one register after the
   jumps.  The consumer reloads the spilled value: x86-64 reads it in
   its slot where no register is named, addl -24(%rbp), %eax after a
   store's value; a return names eax, and the && value takes ecx,
   movl $1, %ecx, then movq -24(%rbp), %rax and addl %ecx, %eax;
   aarch64 reloads into the next free register, ldr x1, [x29, #32] and
   add w0, w1, w0.  - and < reload in any case.  A value on the left
   holds nothing at its paths; two values spill the first before the
   second's.  A left operand generated before a right one that holds
   || keeps no register out of its rows' choice for the % below the
   ||, across which no value is held: chain_over_logical() adds its
   chain's spilled operand into eax, addl -48(%rbp), %eax, where
   keeping out the rax and rdx the % pins would reload it into rcx.

   Each function holds a value across one && or ||, its name the
   form. */
int
returned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + (b && c);
}

int
assigned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        c = a + (b && c);
        return c;
}

int
initialized(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int x = a + (b || c);
        return x;
}

int
condition(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a + (b && c))
                return 1;
        return 0;
}

int
discarded(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        a + (b && c);
        return 0;
}

int
two_held(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + (b + (b && c));
}

int
left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (b && c) + a;
}

int
both(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a && b) + (b && c);
}

int
difference(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a - (b || c);
}

int
comparison(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a < (b && c);
}

int
product(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a * (b && c);
}

int
quotient(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a / (b && c);
}

int
shifted(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a << (b || c);
}

int
nested(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a + (b && (c + (a || b)));
}

int
in_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (
            c +
            (c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c + (c + (c + (c + (c + (c + (c + (a && b)))))))))))))))));
}

int
over_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a +
               (b &&
                (c +
                 (c +
                  (c +
                   (c +
                    (c +
                     (c +
                      (c +
                       (c +
                        (c +
                         (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))));
}

int
chain_over_logical(void)
{
        int a = 3;
        int b = 5;
        int c = 7;
        int d = 11;
        a = (a ^
             (b *
              (d |
               (c +
                (d -
                 (d | (c - (a + (d - (a | (d + (d * (c * (c - c)))))))))))))) +
            (a || b % c);
        return a;
}
