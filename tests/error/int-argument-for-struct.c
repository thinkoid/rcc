/* An int passed for a struct parameter: a record takes only a record
   compatible with it, 6.5.17.2p1 through 6.5.3.3p2, an error before
   the passing by value rcc lacks. */
struct point
{
        int x, y;
};

int use(struct point);

int
main(void)
{
        return use(1);
}
