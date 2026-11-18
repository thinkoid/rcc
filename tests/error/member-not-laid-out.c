/* A member access into a struct with a bit-field, which rcc does not
   lay out yet: refused by the bit-field's words. */
struct flags
{
        int count;
        unsigned dirty : 1;
};

int
first(struct flags *p)
{
        return p->count;
}

int
main(void)
{
        return 0;
}
