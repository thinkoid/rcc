/* A label's name has function scope, 6.2.1p3, so a block does not hide
   it: a label of the name in an inner block is defined a second time,
   an error at its name. */
int
main(void)
{
        int a = 1;
L:
        a = 1;
        {
        L:
                return a;
        }
}
