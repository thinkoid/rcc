/* An array decays to a pointer, the operand of no '*'. */
int
f(void)
{
        int a[3];
        return a * 2;
}
