/* A local struct initialized by a list, whose members are stores at run
   time: not yet, as the initializer of any local record is. */
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
