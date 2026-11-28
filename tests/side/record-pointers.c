/* Pointers to records, 6.2.5p27, as any pointer: declared, compared,
   assigned, passed, returned and tested; p + n and p - n step by the
   record's size, the stride, p - q divides by it, 6.5.7p9, p10, and
   &p[i] is the element's address, 6.5.4.2p3, by a constant at any
   stride and by a variable at 2, 4 and 8 bytes; &*p is the pointer's
   value. */
struct s
{
        int a;
        long b;
};

struct s *
same(struct s *p)
{
        return p;
}

int
equal(struct s *p, struct s *q)
{
        return p == q;
}

int
less(struct s *p, struct s *q)
{
        return p < q;
}

int
null(struct s *p)
{
        return p != 0 || !p;
}

int
truth(struct s *p)
{
        if (p)
                return 1;
        return 0;
}

struct s *
assigned(struct s *p, struct s *q)
{
        q = p;
        return q;
}

struct s *
passed(struct s *p)
{
        return same(p);
}

struct s *
next(struct s *p)
{
        return p + 1;
}

struct s *
first(struct s *p)
{
        return 1 + p;
}

struct s *
back(struct s *p)
{
        return p - 2;
}

long
apart(struct s *p, struct s *q)
{
        return p - q;
}

struct s *
element(struct s *p)
{
        return &p[3];
}

struct s *
self(struct s *p)
{
        return &*p;
}

unsigned long
cast(struct s *p)
{
        return (unsigned long) p;
}

int *
recast(struct s *p)
{
        return (int *) p;
}

struct w
{
        int a;
};

struct e
{
        long l;
};

struct h
{
        char a, b;
};

struct w *
word(struct w *p, int i)
{
        return &p[i];
}

struct w *
words(struct w *p, long i)
{
        return i + p;
}

struct e *
eight(struct e *p, unsigned i)
{
        return p - i;
}

struct h *
half(struct h *p, int i)
{
        return p + i;
}

long
halves(struct h *p, struct h *q)
{
        return p - q;
}
