/* A function declared without static, then defined static: the first
   declaration gives the name external linkage, 6.2.2p5, the second
   internal, p3, and a name of both linkages in one unit is undefined,
   p7. */
int twice(int x);

static int
twice(int x)
{
        return x + x;
}

int
main(void)
{
        return twice(1);
}
