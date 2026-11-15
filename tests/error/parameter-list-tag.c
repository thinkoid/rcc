/* A tag first written in a prototype's parameter list has the scope of
   the list, 6.2.1p4, which ends at its ')': the struct S at file scope
   is another type, and the two declarations of f have conflicting
   types. */
void f(struct S *);

struct S
{
        int x;
};

void
f(struct S *p)
{
}

int
main(void)
{
        return 0;
}
