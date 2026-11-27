/* A parameter assigned is a local's store row: movl $5, -4(%rbp) and
   mov w0, #5 with str w0, [x29, #16].  On x86-64 the seventh is
   written where the caller pushed it, movl $5, 16(%rbp); on aarch64
   the ninth is written to its copy's slot.  A chain stores the
   innermost first. */
int
first(int a)
{
        a = 5;
        a = a + 1;
        return a;
}

int
seventh(int a, int b, int c, int d, int e, int f, int g)
{
        g = 5;
        g = g + a;
        return g;
}

int
ninth(int a, int b, int c, int d, int e, int f, int g, int h, int i)
{
        i = a + i;
        return i;
}

int
chain(int a, int b)
{
        a = b = 3;
        return a + b;
}
