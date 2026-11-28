/* A member of type short through a pointer, 6.5.3.4p4: the object of
   its type at the member's offset from what the pointer points to, read
   as the object through a pointer of its type is, at its width, as a
   value, converted, and at offset 0. */
struct s
{
        long x;
        short m;
};

struct z
{
        short m;
        long x;
};

short
read(struct s *p)
{
        return p->m;
}

long
widened(struct s *p)
{
        return (long) p->m;
}

short
first(struct z *p)
{
        return p->m;
}

int
tested(struct s *p)
{
        if (p->m)
                return 1;
        return p->m == 0;
}

short
through(struct s *p)
{
        return (*p).m;
}
