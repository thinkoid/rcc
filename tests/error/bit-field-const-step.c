/* A step of a const bit-field that does not fill its access unit: its
   operand is no modifiable lvalue (6.5.3.5p1), an error. */
struct counter
{
        unsigned low : 4;
        const unsigned high : 4;
};

void
bump(struct counter *p)
{
        p->high++;
}

int
main(void)
{
        return 0;
}
