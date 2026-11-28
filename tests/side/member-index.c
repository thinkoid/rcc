/* A member of an element, p[i].m, 6.5.3.2p2, 6.5.3.4p4: the element's
   address, the row, plus the member's offset; a constant index's row is
   a displacement at any size of the record, 1, 2, 4, 6, 8, 12, 16, 24
   and 32 bytes, and a variable index's is the machine's scaled index at
   2, 4 and 8 bytes, of an int, a long and an unsigned index, written
   first and second; each read, written and addressed. */
struct r1
{
        char m;
};

struct r2
{
        char a, m;
};

struct r4
{
        short a, m;
};

struct r6
{
        short a, b, m;
};

struct r8
{
        int a;
        int m;
};

struct r12
{
        int a, b, m;
};

struct r16
{
        long a;
        int m;
};

struct r24
{
        long a, b;
        int m;
};

struct r32
{
        long a, b, c;
        int m;
};

char
c1(struct r1 *p)
{
        return p[3].m;
}

char
c2(struct r2 *p)
{
        return p[3].m;
}

short
c4(struct r4 *p)
{
        return p[3].m;
}

short
c6(struct r6 *p)
{
        return p[3].m;
}

int
c8(struct r8 *p)
{
        return p[3].m;
}

int
c12(struct r12 *p)
{
        return p[3].m;
}

int
c16(struct r16 *p)
{
        return p[3].m;
}

int
c24(struct r24 *p)
{
        return p[3].m;
}

int
c32(struct r32 *p)
{
        return p[-2].m;
}

void
w32(struct r32 *p, int v)
{
        p[5].m = v;
}

int *
a24(struct r24 *p)
{
        return &p[1].m;
}

char
v2(struct r2 *p, int i)
{
        return p[i].m;
}

short
v4(struct r4 *p, int i)
{
        return p[i].m;
}

int
v8(struct r8 *p, int i)
{
        return p[i].m;
}

int
l8(struct r8 *p, long i)
{
        return p[i].m;
}

int
u8(struct r8 *p, unsigned i)
{
        return p[i].m;
}

int
f8(struct r8 *p, int i)
{
        return i[p].m;
}

void
w8(struct r8 *p, int i, int v)
{
        p[i].m = v;
}

int *
a8(struct r8 *p, int i)
{
        return &p[i].m;
}

int
z8(struct r8 *p, int i)
{
        return p[i].a;
}

short
s4(struct r4 *p, int i, int j)
{
        return p[i].m + p[j].a;
}
