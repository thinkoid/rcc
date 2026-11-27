/* A function's name multiplied: the operands of * are arithmetic,
   6.5.6p2, and its value is a pointer. */
int g(void);

int
f(void)
{
        return g * 2;
}
