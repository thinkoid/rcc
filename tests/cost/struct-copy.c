/* shape.md chapter 4: a record's assignment, 6.5.17.2, the two lvalues
   of one record, which c1 copies the right's object into the left's
   by: through two pointers, between globals, between a local and a
   parameter's object, between a member of a record type and an
   element of an array of records by a variable index, of a union, and
   in each place a value is not wanted, a statement, a for's clauses,
   the left operand of a comma and the operand of a cast to void.  The
   record's size and alignment are 8 and 4 for point, 24 and 8 for
   wide, 3 and 1 for bytes and 8 and 8 for either. */
struct point
{
        int x, y;
};

struct wide
{
        long a;
        char b;
        long c;
};

struct bytes
{
        char a, b, c;
};

struct line
{
        struct point from, to;
};

union either
{
        char c;
        long l;
};

struct point origin, corner;
struct wide first, second;
struct bytes three;
union either one, other;

void
through(struct point *to, struct point *from)
{
        *to = *from;
}

void
globals(void)
{
        origin = corner;
        first = second;
}

int
local(struct bytes *p)
{
        struct bytes b;

        b = *p;
        three = b;

        return b.c;
}

void
member(struct line *l, struct point *points, int i)
{
        l->to = points[i];
        points[i + 1] = l->from;
}

void
unions(void)
{
        one = other;
}

int
effects(struct point *points, int n)
{
        int i;

        for (origin = points[0], i = 0; i < n; corner = points[i], i++)
                ;

        (void) (origin = corner);

        return (corner = origin, corner.x);
}
