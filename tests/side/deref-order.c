/* The order of evaluation, G13: the place first, held across its value,
   spilled across a call and the paths of && || and ?:, and formed
   outside the registers a division or a shift pins. */
int g(void);
int h(void);
int *gp(void);

int
call_value(int *p)
{
        *p = g();
        return 0;
}

int
call_value_used(int *p)
{
        return *p = g();
}

int
call_place(int x)
{
        *gp() = x;
        return 0;
}

int
call_place_and_value(void)
{
        *gp() = h();
        return 0;
}

int
pointer_to_pointer(int **pp)
{
        **pp = g();
        return 0;
}

int
division(int *p, int a, int b)
{
        *p = a / b;
        return *p = a % b;
}

int
shift(int *p, int a, int b)
{
        *p = a << b;
        return *p = a >> b;
}

int
logical(int *p, int a, int b)
{
        *p = a && b;
        return *p = a || b;
}

int
conditional(int *p, int c, int a, int b)
{
        *p = c ? a : b;
        return 0;
}
