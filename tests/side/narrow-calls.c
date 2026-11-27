/* Calls with narrow values: an argument of a narrow type converted to
   its int parameter, its object read by its extending load, movsbl
   -1(%rbp), %eax, and stored to the argument's slot; a function of a
   narrow result, which returns the promoted value; and a call's int
   result stored into a narrow local. */
int g(int x);

int
argument(void)
{
        char c = 1;
        return g(c);
}

int
arguments(void)
{
        unsigned char c = 1;
        short s = 2;
        return g(c + s) + g((char) s);
}

char
result(void)
{
        int a = 300;
        return a;
}

unsigned short
result_unsigned(void)
{
        long a = 70000;
        return a;
}

int
stored(void)
{
        char c;
        c = g(1);
        return c;
}
