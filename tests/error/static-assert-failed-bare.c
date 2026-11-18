/* A static assertion without its string, C23's form, 6.7.12p1, whose
   expression compares equal to 0, p2, in a block. */
int
main(void)
{
        _Static_assert(sizeof(long) < sizeof(int));

        return 0;
}
