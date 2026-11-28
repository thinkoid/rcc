/* A comma follows its left operand, 6.5.18p1: a ',' first is a
   missing expression. */
int
main(void)
{
        int a = 1;
        return (, a);
}
