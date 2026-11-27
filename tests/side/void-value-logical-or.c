/* A void call's value as the left operand of ||. */
void g(void);

int
f(int a)
{
        return g() || a;
}
