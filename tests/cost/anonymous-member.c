/* shape.md chapter 3: an anonymous member's members are the enclosing
   record's, 6.7.3.2p15, at the anonymous member's offset plus their
   own, so an access through one is the access of a member at that sum.
   struct value holds an anonymous union at 8 after an int, of an int
   and a long, and an anonymous struct after it, of a short and an int,
   at 16 and 20; union cell holds an anonymous struct of two ints over
   a long.  read_union() and write_union() read and write p->l at 8,
   read_struct() and write_struct() p->h at 16 and p->n at 20, the
   local and the global the same members of the slot and of the label,
   and both() the two ints of a union's anonymous struct at 0 and 4.  A
   list gives an anonymous member its values as it gives a member of
   its record's type, by brace elision, 6.7.11p21: preset's data is 1,
   2 at 8 and 3 and 4 at 16 and 20, and listed()'s local the stores of
   5 and 6 at 0 and 4 after its zeroing.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with each anonymous member named and
   each access written through its name.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
struct value
{
        int kind;
        union
        {
                int i;
                long l;
        };
        struct
        {
                short h;
                int n;
        };
};

union cell
{
        struct
        {
                int lo, hi;
        };
        long whole;
};

struct value g;

long
read_union(struct value *p)
{
        return p->l;
}

void
write_union(struct value *p, long v)
{
        p->l = v;
}

int
read_struct(struct value *p)
{
        return p->h + p->n;
}

void
write_struct(struct value *p, int v)
{
        p->h = v;
        p->n = v;
}

int
local(void)
{
        struct value s;

        s.i = 1;
        s.n = 2;
        return s.i + s.n;
}

long
global(void)
{
        g.l = 3;
        return g.l + g.h;
}

int
both(union cell *c)
{
        c->lo = 1;
        return c->hi;
}

struct value preset = { 1, 2, 3, 4 };

int
listed(void)
{
        union cell c = { 5, 6 };

        return c.hi;
}
