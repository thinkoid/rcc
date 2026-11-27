/* A function's name assigned to, which is no modifiable lvalue,
   6.5.17.1p2: the error is at the '='. */
int g(void);

int
f(void)
{
        g = 1;
        return 0;
}
