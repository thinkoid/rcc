/* A compound assignment of a bit-field that does not fill its access
   unit, which rcc does not compile yet: refused at its operator. */
struct counter
{
        unsigned low : 4, high : 4;
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
