/* A record assigned whole that holds a member declared const, in a
   member of its own: no modifiable lvalue, 6.3.2.1p1, which counts
   "any member (including, recursively, any member or element of all
   contained aggregates or unions)". */
struct inner
{
        const int id;
};

struct outer
{
        struct inner in;
        int n;
};

void
copy(struct outer *to, struct outer *from)
{
        *to = *from;
}

int
main(void)
{
        return 0;
}
