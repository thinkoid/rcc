/* Conditions whose value spills: a chain of 16 operators nested on the
   right, as a condition, as an operand of a comparison or of &, and as
   an operand of && and ||, which hold no value of their own across it.
   The jump reloads the spilled left operand of a cmp or a test before
   it reads both, the right operand's register asked first:
   movq -24(%rbp), %rax and testl %r11d, %eax on x86-64.  A chain's own top
   operator, its sum or its difference, sets ZF from its value, and the
   jump takes it with no cmp; a product does not, and is compared with
   0, as on aarch64 every value but a comparison is, by cbz.

   Each function is one if over one condition, its name the
   condition's form. */
int
chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((c -
             (c -
              (c -
               (c -
                (c -
                 (c -
                  (c -
                   (c -
                    (c - (c - (c - (c - (c - (c - (c - (c - b)))))))))))))))))
                return 1;
        return 0;
}

int
chain_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c + (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))) ==
            0)
                return 1;
        return 0;
}

int
less_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a <
            (c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c + (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))))
                return 1;
        return 0;
}

int
chain_less_constant(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((c ^
             (c ^
              (c ^
               (c ^
                (c ^
                 (c ^
                  (c ^
                   (c ^
                    (c ^ (c ^ (c ^ (c ^ (c ^ (c ^ (c ^ (c ^ b)))))))))))))))) <
            4097)
                return 1;
        return 0;
}

int
bitwise_and_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a &
            (c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c + (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))))
                return 1;
        return 0;
}

int
product_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a *
            (c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c + (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))))
                return 1;
        return 0;
}

int
negated_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!(c |
              (c |
               (c |
                (c |
                 (c |
                  (c |
                   (c |
                    (c |
                     (c | (c | (c | (c | (c | (c | (c | (c | b)))))))))))))))))
                return 1;
        return 0;
}

int
chain_and_local(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c + (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))) &&
            a)
                return 1;
        return 0;
}

int
local_or_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a ||
            (c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c + (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))))
                return 1;
        return 0;
}
