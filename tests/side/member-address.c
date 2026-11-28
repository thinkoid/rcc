/* & of a member, 6.5.4.2p3: the address of the member's place, the
   pointer plus its offset, a pointer to the member's type; of a member
   at offset 0, the pointer's value; of a record member, a pointer to
   its record; and each compared and dereferenced. */
struct s
{
        long x;
        int m;
        int *q;
};

struct z
{
        int m;
        long x;
};

struct t
{
        int k;
        struct s in;
};

int *
address(struct s *p)
{
        return &p->m;
}

int *
first(struct z *p)
{
        return &p->m;
}

struct s *
inner(struct t *p)
{
        return &p->in;
}

int *
nested(struct t *p)
{
        return &p->in.m;
}

int **
pointer(struct s *p)
{
        return &p->q;
}

int
compared(struct s *p, int *r)
{
        return &p->m == r;
}

int
read(struct s *p)
{
        return *&p->m;
}

int *
element(struct s *p)
{
        return &p[2].m;
}
