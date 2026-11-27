/* extern on a function changes nothing, 6.2.2p5: on a declaration and
   on a definition, f is of external linkage, and g, declared static
   first, keeps its internal linkage. */
extern int f(void);

extern int
f(void)
{
        return 1;
}

static int g(void);
extern int g(void);

extern int
g(void)
{
        return 2;
}

int
h(void)
{
        return f() + g();
}
