/* Two declarators of one extern declaration. */
int
f(void)
{
        extern int e, d;
        return e + d;
}
