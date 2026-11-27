/* Binary >>, 6.5.8p5, in the six shapes of left-shift.c's <<: the same
   rows with sarl and asr, the arithmetic shift, which is G3's for a
   negative int, the implementation's choice by 6.5.8p5.

   both() shifts a local by a local, right_constant() by a constant,
   left_constant() a constant by a local; nested_left(), nested_right()
   and nested_both() nest a shift on the left, on the right and on
   both. */
int
both(void)
{
        int a = -96;
        int b = 3;
        return a >> b;
}

int
right_constant(void)
{
        int a = -96;
        return a >> 5;
}

int
left_constant(void)
{
        int a = 2;
        return 500 >> a;
}

int
nested_left(void)
{
        int a = 2;
        int b = 3;
        return (a >> b) >> a;
}

int
nested_right(void)
{
        int a = 1;
        int b = 3;
        return a >> (b >> a);
}

int
nested_both(void)
{
        int a = -96;
        int b = 1;
        return (a >> b) >> (a >> b);
}
