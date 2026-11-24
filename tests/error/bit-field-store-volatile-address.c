/* A store of a value into a bit-field that does not fill its access
   unit, through a place whose address reads a volatile object, which
   rcc does not compile yet: the store writes the place twice, and the
   read of the volatile pointer d is a side effect, 5.1.2.4p2, which the
   source writes once. */
struct small
{
        unsigned a : 3;
        int f : 5;
};

struct small *volatile d;

void
store(int v)
{
        d->f = v;
}

int
main(void)
{
        return 0;
}
