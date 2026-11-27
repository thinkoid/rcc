/* A comparison with a value spilled: a right operand held in more
   registers than the target has spills the left operand, and the cmp
   reloads it, all 8 bytes, the right operand's register asked first,
   as for any binary operator: less_chain_right() is ldr x0, [x29, #32]
   and cmp w0, w1 on aarch64, movq -24(%rbp), %rax and cmpl %r11d, %eax
   on x86-64.  The value of the flags goes to the register the consumer
   names: a left operand left in w1 is cmp w1, w0 and cset w0, lt, and
   x86-64's setl writes al.  A comparison inside a chain is one more
   value held, and ! of a chain is its test, cmp w1, #0 and cset w0, eq.

   Each function returns one comparison, or ! of one value, over a
   chain of 16 operators nested on the right, its name the form. */
int
less_chain_right(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a <
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c +
                     (c +
                      (c +
                       (c +
                        (c + (c + (c + (c + (c + (c + (c + b))))))))))))))));
}

int
less_chain_left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c +
                     (c +
                      (c +
                       (c +
                        (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))) <
               a;
}

int
equal_chain_right(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a ==
               (c -
                (c -
                 (c -
                  (c -
                   (c -
                    (c -
                     (c -
                      (c -
                       (c -
                        (c - (c - (c - (c - (c - (c - (c - b))))))))))))))));
}

int
less_chain_constant(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c +
                     (c +
                      (c +
                       (c +
                        (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))) <
               4097;
}

int
negated_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return !(
            c +
            (c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c + (c + (c + (c + (c + (c + (c + (c + b))))))))))))))));
}

int
comparisons_in_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (
            a +
            (c <
             (b +
              (a <
               (c +
                (b <
                 (a +
                  (c <
                   (b + (a < (c + (b < (a + (c < (b + (a < c))))))))))))))));
}

int
assigned_comparison(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int x;
        x = a <
            (c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c + (c + (c + (c + (c + (c + (c + (c + b))))))))))))))));
        return x;
}
