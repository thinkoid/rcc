/* An anonymous member's members are the record's, 6.7.3.2p15, each at
   the anonymous member's offset plus its own, at any depth, of a struct
   in a union and a union in a struct. */
struct s
{
        int k;
        struct
        {
                char c;
                long l;
        };
        union
        {
                int i;
                char d;
        };
};

struct n
{
        struct
        {
                struct
                {
                        int deep;
                };
        };
        int e;
};

union u
{
        struct
        {
                int a;
                int b;
        };
        long l;
};

long
read(struct s *p)
{
        return p->l + p->d + p->k;
}

void
write(struct s *p, int v)
{
        p->i = v;
        p->c = v;
}

int
deep(struct n *p)
{
        return p->deep + p->e;
}

int
inner(union u *p)
{
        return p->b;
}
