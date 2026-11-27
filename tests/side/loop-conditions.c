/* A loop's test is the jump code of its condition: a while's and a
   for's jump past the loop where it is 0, at the head; a do's jumps
   back to the body where it is nonzero, at the foot, so a top && there
   takes a label and a top || none, where a while's takes the other.
   An assignment, a comparison and a constant as conditions are their
   own jumps, and && or || in a for's init or step is a value.

   Each function is one loop, its name the loop and its condition. */
int
while_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a && b)
                a = 0;
        return 0;
}

int
while_disjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a || b)
                a = 0;
        return 0;
}

int
do_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                ;
        while (a && b);
        return 0;
}

int
do_disjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                ;
        while (a || b);
        return 0;
}

int
do_and_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                ;
        while (a && b || c);
        return 0;
}

int
do_or_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                ;
        while ((a || b) && c);
        return 0;
}

int
do_comparison(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                ;
        while (a < b);
        return 0;
}

int
do_mirrored(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                ;
        while (5 < a);
        return 0;
}

int
do_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                ;
        while (a == 0);
        return 0;
}

int
do_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                a = a - 1;
        while (a + b);
        return a;
}

int
for_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (; a && b;)
                ;
        return a;
}

int
for_init_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = b && c; a < 10; a = a + 1)
                ;
        return a;
}

int
for_step_conjunction(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (; a < 10; a = b && c)
                ;
        return a;
}

int
while_assignment(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a = b)
                b = b - 1;
        return a;
}

int
while_block(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a < 10) {
                a = a + 1;
        }
        return a;
}

int
do_block(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do {
                a = a + 1;
        } while (a < 10);
        return a;
}
