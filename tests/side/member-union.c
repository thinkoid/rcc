/* A union's members are all at its start, 6.7.3.2p18, so each is read
   and written through a pointer at offset 0, at its own width. */
union u
{
        char c;
        short h;
        int i;
        long l;
        int *q;
};

struct s
{
        int k;
        union u v;
};

int
read(union u *p)
{
        return p->c + p->h + p->i;
}

long
wide(union u *p)
{
        return p->l;
}

void
write(union u *p, int v)
{
        p->h = v;
        p->i = v;
}

int
inner(struct s *p)
{
        return p->v.i + p->k;
}

int *
pointer(union u *p)
{
        return p->q;
}

unsigned long
size(union u *p)
{
        return sizeof *p + sizeof p->c;
}
