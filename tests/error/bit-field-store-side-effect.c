/* A store of a value into a bit-field that does not fill its access
   unit, through a place with a side effect, which rcc does not compile
   yet: the store writes the place twice, and a place with a side
   effect has no copy.  A store of 0 or of all ones writes it once, and
   compiles. */
struct flags
{
        int count;
        unsigned dirty : 1, level : 3;
};

void
mark(struct flags *f, int i)
{
        f[i++].dirty = 1;
}

void
raise(struct flags *f, int i, int v)
{
        f[i++].level = v;
}

int
main(void)
{
        return 0;
}
