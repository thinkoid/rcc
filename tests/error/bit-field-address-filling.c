/* The address of a bit-field that fills its access unit, which is read
   and written as an object of the unit's type and is still a
   bit-field: the operand of & is no bit-field, 6.5.4.2p1. */
struct job
{
        int stop;
        unsigned nprocs : 16, state : 8;
};

void *
state(struct job *p)
{
        return &(p->state);
}

int
main(void)
{
        return 0;
}
