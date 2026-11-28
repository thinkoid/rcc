/* A size folded from a conditional of constants is a constant, as in
   rcc. */
unsigned long
f(void)
{
        int e[1 ? 2 : 3];
        return sizeof e;
}
