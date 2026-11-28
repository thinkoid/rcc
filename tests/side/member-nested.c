/* A member of a member is one offset from the outermost pointer, rcc's
   record_pointer(): p->in.m and (&p->in)->m are the member at the sum
   of the two offsets; a member through a pointer member, p->q->m; a
   member of a call's result, h()->m; a member of *p and of p[0]. */
struct s
{
        long x;
        int m;
};

struct t
{
        int k;
        struct s in;
};

struct u
{
        long y;
        struct t t;
};

struct q
{
        int k;
        struct s *p;
};

struct s *h(void);

int
nested(struct t *p)
{
        return p->in.m;
}

int
deeper(struct u *p)
{
        return p->t.in.m + p->t.k;
}

int
addressed(struct t *p)
{
        return (&p->in)->m;
}

int
deref(struct t *p)
{
        return (*p).in.x;
}

int
through(struct q *r)
{
        return r->p->m;
}

void
stored(struct q *r, int v)
{
        r->p->m = v;
}

int
called(void)
{
        return h()->m;
}

int
zeroth(struct t *p)
{
        return p[0].in.m + (*p).k;
}

unsigned long
size(struct t *p)
{
        return sizeof p->in + sizeof p->in.m;
}
