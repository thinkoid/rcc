/* A function's name as an int argument, 6.5.3.3p2: the error is at the
   argument's first token. */
int g(void);
int h(int a);

int
f(void)
{
        return h(g);
}
