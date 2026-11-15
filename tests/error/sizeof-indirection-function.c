/* sizeof applied to *p on a pointer to a function: the operand is a
   function designator, which has no size, 6.5.4.4p1. */
unsigned long
size(int (*p)(void))
{
        return sizeof *p;
}

int
main(void)
{
        return 0;
}
