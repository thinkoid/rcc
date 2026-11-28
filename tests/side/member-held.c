/* A member's place held across its right operand, rcc's place_of():
   across a call, a division, a variable shift, && and ?:, the member's
   offset in the store's displacement after it; a row summed before it;
   and a member read on the left of a call. */
struct s
{
        long x;
        int m;
};

int g(void);

void
call(struct s *p)
{
        p->m = g();
}

void
indexed_call(struct s *p)
{
        p[3].m = g();
}

void
divided(struct s *p, int a, int b)
{
        p->m = a / b;
}

void
shifted(struct s *p, int a, int b)
{
        p->m = a << b;
}

void
logical(struct s *p, int a, int b)
{
        p->m = a && b;
}

void
conditional(struct s *p, int c, int a, int b)
{
        p->m = c ? a : b;
}

int
left(struct s *p)
{
        return p->m + g();
}

int
right(struct s *p)
{
        return g() + p->m;
}

int
value(struct s *p)
{
        return p->m = g();
}

struct q
{
        int k;
        struct s *p;
};

void
inner(struct q *r)
{
        r->p->m = g();
}
