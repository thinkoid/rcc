/* A local struct initialized by a list, whose members are stores at run
   time: not yet, where an initializer that is a struct's value is the
   copy. */
struct point
{
        int x, y;
};

int
main(void)
{
        struct point q = { 1, 2 };

        return 0;
}
