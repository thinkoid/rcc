/* A read of a bit-field, which rcc does not compile yet: refused at the
   member's name.  The record is laid out, so its other members are
   reached. */
struct flags
{
        int count;
        unsigned dirty : 1;
};

int
count(struct flags *p)
{
        return p->count;
}

int
dirty(struct flags *p)
{
        return p->dirty;
}

int
main(void)
{
        return 0;
}
