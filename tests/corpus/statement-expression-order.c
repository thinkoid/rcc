/* G13: a statement expression's statements run where it stands, after
   the operands to its left and before those to its right.  mark()
   records the number it is called with, in the order of the calls,
   and returns it; each check evaluates an expression whose operands
   and whose statement expressions call it, and compares the record
   with the order G13 promises and the value with the expression's.
   The checks: the operands of +, a statement expression's block
   holding a call beside a value held, the arguments of a call, a
   statement expression nested in another, an assignment's place
   before its right operand, the operands of && and of ?:, and a
   statement expression as a condition.  It exits 42, or 100 plus the
   number of the first check that failed.

   Under rcc it exits 42.  ISO C leaves the order unspecified where the
   operands of an operator, the arguments of a call and the operands of
   an assignment are unsequenced (6.5.1p3, 6.5.3.3p8, 6.5.17.1p3), and
   clang -O2 is free to take another; && and ?: sequence their first
   operand before the rest (6.5.14p4, 6.5.16p5) under both.

   On x86-64 and on aarch64 the listing must show each mark() called in
   the order of the numbers it is called with. */
int order[16], marks;

int
mark(int n)
{
        order[marks++] = n;

        return n;
}

int
three(int a, int b, int c)
{
        return a * 100 + b * 10 + c;
}

/* whether the record holds the numbers 1 to count, in order */
int
in_order(int count)
{
        int i;

        if (marks != count)
                return 0;

        for (i = 0; i < count; i++)
                if (order[i] != i + 1)
                        return 0;

        marks = 0;

        return 1;
}

int
main(void)
{
        int a[4];
        int n;

        n = mark(1) + ({
                    mark(2);
                    3;
            }) +
            mark(3);
        if (!in_order(3) || n != 7)
                return 101;

        n = mark(1) * 10 + ({
                    int t = mark(2);
                    mark(3);
                    t;
            });
        if (!in_order(3) || n != 12)
                return 102;

        n = three(mark(1), ({
                          mark(2);
                          5;
                  }),
                  mark(3));
        if (!in_order(3) || n != 153)
                return 103;

        n = mark(1) + ({
                    int t = mark(2) + ({
                                    mark(3);
                                    4;
                            });
                    mark(4);
                    t;
            });
        if (!in_order(4) || n != 7)
                return 104;

        a[mark(1)] = ({
                mark(2);
                9;
        });
        if (!in_order(2) || a[1] != 9)
                return 105;

        n = mark(1) && ({
                    mark(2);
                    0;
            }) &&
            mark(3);
        if (!in_order(2) || n != 0)
                return 106;

        n = mark(1) ? ({
                mark(2);
                6;
        }) :
                      mark(9);
        if (!in_order(2) || n != 6)
                return 107;

        if (({
                    mark(1);
                    mark(2);
            }) == 2)
                n = mark(3);
        if (!in_order(3) || n != 3)
                return 108;

        return 42;
}
