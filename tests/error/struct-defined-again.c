/* A struct defined twice in one scope.  The two are one type when
   their content is the same, which rcc cannot compare while it skips
   the member lists: not yet. */
struct point
{
        int x;
        int y;
};
struct point
{
        int x;
        int y;
};

int
main(void)
{
        return 0;
}
