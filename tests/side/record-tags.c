/* A tag is declared in the scope of its specifier, 6.7.3.4, in a name
   space of its own, 6.2.3p1: a block's list hides the file's tag, which
   is seen again after the block; struct s; in a block declares a new
   incomplete type there, p11; a tag a parameter list declares is the
   list's and the body's; a tag first met behind a pointer declares an
   incomplete type, which a later list completes, p5; a list in a member
   declaration declares its tag where the record is; and a tag is no
   ordinary identifier, so an object of its name stands beside it. */
struct s
{
        int a;
        long b;
};

unsigned long
hidden(void)
{
        struct s
        {
                char c;
        };
        return sizeof(struct s);
}

unsigned long
seen_again(void)
{
        return sizeof(struct s);
}

unsigned long
declared_inner(struct s *p)
{
        struct s;
        struct s *q;
        return sizeof q + sizeof *p;
}

int parameter(struct t {
        int x;
        int y;
} * p)
{
        return sizeof(struct t) + (p != 0);
}

struct later *
forward(struct later *p)
{
        return p;
}

struct later
{
        int z;
};

unsigned long
completed(struct later *p)
{
        return sizeof *p;
}

struct outer
{
        struct inner
        {
                int k;
        } *in;
        int n;
};

unsigned long
member_tag(struct inner *p)
{
        return sizeof *p + sizeof(struct outer);
}

typedef struct
{
        short h;
        char c;
} T;

unsigned long
typedefed(T *p)
{
        return sizeof(T) + sizeof *p;
}

union u
{
        char c;
        long l;
        int i;
};

unsigned long
unioned(union u *p)
{
        return sizeof(union u) + sizeof *p;
}

int
beside(struct s *s)
{
        return s != 0;
}
