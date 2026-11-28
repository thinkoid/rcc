/* *p written: the place first, its address formed before the value and
   held across it, then the value's store at it, movl $7, (%rax) and
   str w1, [x0], or the zero register's str wzr, [x0]; a returned value
   with the place formed outside the result register. */
int g(void);
int *gp(void);

int
constant(int *p)
{
        *p = 7;
        return 0;
}

int
zero(int *p)
{
        *p = 0;
        return 0;
}

int
local(int *p, int x)
{
        *p = x;
        return 0;
}

int
copied(int *p, int *q)
{
        *p = *q;
        return 0;
}

int
copied_used(int *p, int *q)
{
        return *p = *q;
}

int
chain(int *p, int *q)
{
        *p = *q = 3;
        return 0;
}

int
expression(int *p, int x, int y)
{
        *p = x + y;
        return 0;
}

int
returned(int *p)
{
        return *p = 7;
}

int
pointer_stored(int **pp, int *q)
{
        *pp = q;
        **pp = 5;
        *pp = 0;
        return 0;
}

int
through_call(int x)
{
        *gp() = x;
        return 0;
}

long
widened_store(int *p, long l)
{
        *p = l;
        return *p = l + 1;
}

int
in_place_of_local(void)
{
        int x = 1;
        int *p = &x;
        x = 2;
        return x + *p;
}
