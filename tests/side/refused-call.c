/* A function of a long result is declared and defined as any other,
   but a call of it is not yet compiled, refused at the call's '('. */
long g(void);

long
g(void)
{
        return 1;
}

int
f(void)
{
        return g();
}
