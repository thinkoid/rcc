/* A member's offset is the access's displacement wherever the target's
   form holds it: aarch64 encodes a multiple of the access's size up to
   4095 times it, 4095 for a byte and 16380 for an int, and past it adds
   the offset first; x86-64 holds any of 32 bits. A negative member,
   p[-1].m, is a displacement below the pointer. */
struct l8
{
        long a0, a1, a2, a3, a4, a5, a6, a7;
};

struct l64
{
        struct l8 b0, b1, b2, b3, b4, b5, b6, b7;
};

struct b
{
        struct l64 c0, c1, c2, c3, c4, c5, c6;
        struct l8 d0, d1, d2, d3, d4, d5, d6;
        long e0, e1, e2, e3, e4, e5, e6;
        char f0, f1, f2, f3, f4, f5, f6;
        char m;
        char n;
};

struct i
{
        struct l64 c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13,
            c14, c15, c16, c17, c18, c19, c20, c21, c22, c23, c24, c25, c26,
            c27, c28, c29, c30;
        struct l8 d0, d1, d2, d3, d4, d5, d6;
        long e0, e1, e2, e3, e4, e5, e6;
        int k;
        int m;
        int n;
};

struct s
{
        long x;
        int m;
};

char
byte(struct b *p)
{
        return p->m;
}

char
past(struct b *p)
{
        return p->n;
}

int
word(struct i *p)
{
        return p->m;
}

int
beyond(struct i *p)
{
        return p->n;
}

void
store(struct i *p, int v)
{
        p->n = v;
}

int
below(struct s *p)
{
        return p[-1].m;
}

int
far(struct s *p)
{
        return p[-300].m;
}

unsigned long
size(void)
{
        return sizeof(struct b) + sizeof(struct i);
}
