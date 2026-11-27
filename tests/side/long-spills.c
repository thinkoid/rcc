/* The spill of long values, C8 of the survey: a value spilled is
   stored and reloaded whole, 8 bytes, whatever its type, and a spilled
   left operand of an operator that commutes is read in its slot at the
   operation's width, addq -48(%rbp), %rdi.  A chain of 16 operators and
   more spills on both targets; ints widened to long spill as longs. */

long
sum(void)
{
        long a = 1;
        long b = 2;
        long c = 3;
        long d = 4;
        return (
            b +
            (a +
             (d +
              (c +
               (b +
                (a +
                 (d +
                  (c +
                   (b +
                    (a +
                     (d +
                      (c + (b + (a + (d + (c + (b + (a + a))))))))))))))))));
}

long
product(void)
{
        long a = 1;
        long b = 2;
        long c = 3;
        long d = 4;
        return (
            d *
            (c *
             (b *
              (a *
               (d *
                (c *
                 (b *
                  (a *
                   (d * (c * (b * (a * (d * (c * (b * (a * a))))))))))))))));
}

long
difference(void)
{
        long a = 1;
        long b = 2;
        long c = 3;
        long d = 4;
        return (
            b -
            (a -
             (d -
              (c -
               (b -
                (a -
                 (d -
                  (c -
                   (b -
                    (a -
                     (d -
                      (c - (b - (a - (d - (c - (b - (a - a))))))))))))))))));
}

long
mask(void)
{
        long a = 1;
        long b = 2;
        long c = 3;
        long d = 4;
        return (
            a &
            (d &
             (c &
              (b &
               (a &
                (d &
                 (c &
                  (b &
                   (a &
                    (d & (c & (b & (a & (d & (c & (b & (a & a)))))))))))))))));
}

long
mixed(void)
{
        long a = 1;
        int b = 2;
        long c = 3;
        int d = 4;
        return (
            b +
            (a +
             (d +
              (c +
               (b +
                (a +
                 (d +
                  (c +
                   (b +
                    (a +
                     (d +
                      (c + (b + (a + (d + (c + (b + (a + a))))))))))))))))));
}

long
quotients(void)
{
        long a = 1;
        long b = 2;
        long c = 3;
        long d = 4;
        return (
            d /
            (c /
             (b /
              (a /
               (d /
                (c /
                 (b /
                  (a /
                   (d / (c / (b / (a / (d / (c / (b / (a / a))))))))))))))));
}
