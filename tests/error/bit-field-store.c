/* A store into a bit-field that does not fill its access unit, which
   rcc does not compile yet: refused at the '='.  Its read compiles. */
struct flags
{
        int count;
        unsigned dirty : 1;
};

int
dirty(struct flags *p)
{
        return p->dirty;
}

void
mark(struct flags *p)
{
        p->dirty = 1;
}

int
main(void)
{
        return 0;
}
