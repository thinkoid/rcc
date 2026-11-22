/* The address of a bit-field: the operand of & is no bit-field,
   6.5.4.2p1. */
struct flags
{
        int count;
        unsigned dirty : 1;
};

unsigned *
dirty(struct flags *p)
{
        return &p->dirty;
}

int
main(void)
{
        return 0;
}
