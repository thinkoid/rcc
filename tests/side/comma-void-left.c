/* A comma's left operand is evaluated as a void expression, 6.5.18p2,
   so it may be void, a void function's call; so may its right one,
   where the comma is itself evaluated for its effect. */
void v(void);

int
f(void)
{
        int a = 1;
        return (v(), a);
}

void
g(void)
{
        int a = 1;
        a, v();
}
