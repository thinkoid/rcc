/* The size of a record is its layout's, its padding included,
   6.5.4.4p4, the layout clang's on each target: members in order at
   their alignment, a union's at 0, the size the end rounded up to the
   largest alignment, 6.7.3.2p16 to p19; bit-fields in containers of
   their type, an unnamed one's type counted in the alignment on aarch64
   alone; sizeof *p, of an object through a pointer, and of a typedef
   name. */
struct ci
{
        char c;
        int i;
};
struct cl
{
        char c;
        long l;
};
struct ccc
{
        char a, b, c;
};
struct sc
{
        short s;
        char c;
};
struct lsic
{
        long l;
        short s;
        int i;
        char c;
};
struct cpc
{
        char c;
        int *p;
        char d;
};
struct nest
{
        char c;
        struct
        {
                char d;
                long l;
        } in;
        char e;
};
struct anon
{
        int k;
        union
        {
                char c;
                long l;
        };
        char e;
};
union uci
{
        char c;
        int i;
        long l;
};
struct bits
{
        int a : 3;
        int b : 5;
        unsigned c : 30;
};
struct zero
{
        char c;
        int : 0;
        char d;
};
struct unnamed
{
        char c;
        int : 4;
};
struct wide
{
        char c;
        long f : 40;
        char d;
};
typedef struct
{
        long l;
        char c;
} T;

unsigned long
ci(void)
{
        return sizeof(struct ci);
}

unsigned long
cl(void)
{
        return sizeof(struct cl);
}

unsigned long
ccc(void)
{
        return sizeof(struct ccc);
}

unsigned long
sc(void)
{
        return sizeof(struct sc);
}

unsigned long
lsic(void)
{
        return sizeof(struct lsic);
}

unsigned long
cpc(void)
{
        return sizeof(struct cpc);
}

unsigned long
nest(void)
{
        return sizeof(struct nest);
}

unsigned long
anon(void)
{
        return sizeof(struct anon);
}

unsigned long
uci(void)
{
        return sizeof(union uci);
}

unsigned long
bits(void)
{
        return sizeof(struct bits);
}

unsigned long
zero(void)
{
        return sizeof(struct zero);
}

unsigned long
unnamed(void)
{
        return sizeof(struct unnamed);
}

unsigned long
wide(void)
{
        return sizeof(struct wide);
}

unsigned long
typedefed(void)
{
        return sizeof(T);
}

unsigned long
through(struct lsic *p)
{
        return sizeof *p;
}

unsigned long
product(struct cpc *p)
{
        return sizeof *p * 3;
}
