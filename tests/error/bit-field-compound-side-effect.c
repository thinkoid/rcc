/* A compound assignment of a bit-field that does not fill its access
   unit, through a place with a side effect, which rcc does not compile
   yet: the tree loads the unit from the place and stores it there, so
   the place is written twice, and a place with a side effect has no
   copy. */
struct flags
{
        int count;
        unsigned dirty : 1, level : 3;
};

void
raise(struct flags *f, int i, int v)
{
        f[i++].level += v;
}

int
main(void)
{
        return 0;
}
