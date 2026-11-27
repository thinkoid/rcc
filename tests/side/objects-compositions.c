/* Every composition of one and of two declarations of an object at
   file scope that rcc accepts, each object its own name and followed
   by a function that uses none: the first declaration gives the
   linkage, static internal and any other external, and extern takes
   the earlier one's; the one initializer gives the value, in .data,
   and a definition without one is zero, in .bss; extern alone writes
   nothing. */
int t;

int
f_t(void)
{
        return 0;
}

int i = 1;

int
f_i(void)
{
        return 0;
}

extern int x;

int
f_x(void)
{
        return 0;
}

extern int xi = 2;

int
f_xi(void)
{
        return 0;
}

static int s;

int
f_s(void)
{
        return 0;
}

static int si = 3;

int
f_si(void)
{
        return 0;
}

int t_t;
int t_t;

int
f_t_t(void)
{
        return 0;
}

int t_i;
int t_i = 1;

int
f_t_i(void)
{
        return 0;
}

int t_x;
extern int t_x;

int
f_t_x(void)
{
        return 0;
}

int t_xi;
extern int t_xi = 2;

int
f_t_xi(void)
{
        return 0;
}

int i_t = 1;
int i_t;

int
f_i_t(void)
{
        return 0;
}

int i_x = 1;
extern int i_x;

int
f_i_x(void)
{
        return 0;
}

extern int x_t;
int x_t;

int
f_x_t(void)
{
        return 0;
}

extern int x_i;
int x_i = 1;

int
f_x_i(void)
{
        return 0;
}

extern int x_x;
extern int x_x;

int
f_x_x(void)
{
        return 0;
}

extern int x_xi;
extern int x_xi = 2;

int
f_x_xi(void)
{
        return 0;
}

extern int xi_t = 2;
int xi_t;

int
f_xi_t(void)
{
        return 0;
}

extern int xi_x = 2;
extern int xi_x;

int
f_xi_x(void)
{
        return 0;
}

static int s_x;
extern int s_x;

int
f_s_x(void)
{
        return 0;
}

static int s_xi;
extern int s_xi = 2;

int
f_s_xi(void)
{
        return 0;
}

static int s_s;
static int s_s;

int
f_s_s(void)
{
        return 0;
}

static int s_si;
static int s_si = 3;

int
f_s_si(void)
{
        return 0;
}

static int si_x = 3;
extern int si_x;

int
f_si_x(void)
{
        return 0;
}

static int si_s = 3;
static int si_s;

int
f_si_s(void)
{
        return 0;
}
