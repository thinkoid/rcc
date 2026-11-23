/* A compound assignment of a const bit-field that does not fill its
   access unit: its left operand is no modifiable lvalue (6.5.17.1p2),
   an error. */
struct counter
{
        unsigned low : 4;
        const unsigned high : 4;
};

void
bump(struct counter *p)
{
        p->high += 2;
}

int
main(void)
{
        return 0;
}
