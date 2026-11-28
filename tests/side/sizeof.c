/* sizeof, 6.5.4.4: an unsigned long constant of the operand's size, the
   operand typed and never evaluated, so its assignment stores nothing
   and its call calls nothing. */
int g(void);

unsigned long
of_objects(void)
{
        int x = 1;
        int *p = &x;
        int **pp = &p;
        return sizeof x + sizeof p + sizeof &x + sizeof pp + sizeof(x);
}

unsigned long
of_types(void)
{
        return sizeof(int) + sizeof(int *) + sizeof(int **) + sizeof(long) +
               sizeof(char) + sizeof(unsigned short);
}

int
of_expressions(void)
{
        int x = 1;
        int n = sizeof(x = 5) + sizeof g() + sizeof(x + 1) + sizeof x + 1;
        return x + n;
}

int
compared(void)
{
        int x = 1;
        return x < sizeof x;
}

long
in_arithmetic(void)
{
        long l = 3;
        return l * sizeof l - sizeof(long) / 2;
}
