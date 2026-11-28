/* *p read, rcc's load() of pointed(): the pointer's value, then the
   object at it in the pointer's register, movl (%rax), %eax and ldr w0,
   [x0]; through a local, a parameter, a call's result, a pointer to a
   pointer, for its effect, and widened. */
int *g(void);
int **gg(void);

int
local(void)
{
        int x = 3;
        int *p = &x;
        return *p;
}

int
parameter(int *p)
{
        return *p;
}

int
call(void)
{
        return *g();
}

int
pointer_to_pointer(int **pp)
{
        return **pp;
}

int
through_calls(void)
{
        return **gg() + *g();
}

int
discarded(int *p)
{
        *p;
        return 0;
}

long
widened(int *p)
{
        return *p;
}

unsigned long
widened_unsigned(int *p)
{
        return *p;
}

int *
pointer_read(int **pp)
{
        return *pp;
}

int
seventh(int *a, int *b, int *c, int *d, int *e, int *f, int *g)
{
        return *g + *a;
}

int
ninth(int *a, int *b, int *c, int *d, int *e, int *f, int *g, int *h, int *i)
{
        return *i - *h;
}

int
stored_call(void)
{
        int *p;
        p = g();
        return *p;
}
