/* A member as each operand of an operator, 6.5.6 to 6.5.15: on the left
   and on the right of the arithmetic, a division and a shift, beside a
   constant, two members of one record and of two, in a comparison, as a
   condition, under !, unary - and ~, and as an argument of a call. */
struct s
{
        long x;
        int m;
        unsigned u;
        long l;
        short h;
        char c;
};

int g(int, long);

int
add(struct s *p, int a)
{
        return a + p->m;
}

int
left(struct s *p, int a)
{
        return p->m + a;
}

int
sub(struct s *p, int a)
{
        return a - p->m;
}

int
mul(struct s *p, int a)
{
        return a * p->m;
}

int
constant(struct s *p)
{
        return p->m * 3 + 1;
}

int
quotient(struct s *p, int a)
{
        return a / p->m;
}

int
remainder(struct s *p, int a)
{
        return p->m % a;
}

int
shifted(struct s *p, int a)
{
        return a << p->m;
}

int
masked(struct s *p)
{
        return p->m & 255;
}

unsigned
unsigned_(struct s *p, unsigned a)
{
        return a + p->u;
}

long
wide(struct s *p, long a)
{
        return a + p->l;
}

int
narrow(struct s *p, int a)
{
        return a + p->h + p->c;
}

int
two(struct s *p)
{
        return p->m + p->h;
}

int
pair(struct s *p, struct s *q)
{
        return p->m - q->m;
}

int
less(struct s *p, int a)
{
        return p->m < a;
}

int
equal(struct s *p, struct s *q)
{
        return p->c == q->c;
}

int
zero(struct s *p)
{
        return p->c == 0;
}

int
not(struct s *p)
{
        return !p->m;
}

int
negated(struct s *p)
{
        return -p->m;
}

int
complemented(struct s *p)
{
        return ~p->h;
}

int
logical(struct s *p, int a)
{
        return a && p->m || p->c;
}

int
conditional(struct s *p, int a)
{
        return a ? p->m : p->h;
}

int
argument(struct s *p)
{
        return g(p->m, p->l);
}

int
condition(struct s *p)
{
        while (p->m)
                if (p->c > 1)
                        return 1;
        return 0;
}
