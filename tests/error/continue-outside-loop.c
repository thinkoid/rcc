/* A continue outside a loop, 6.8.7.3p1: an error.  The loop before it
   has ended. */
int
main(void)
{
        int i = 0;

        while (i < 3)
                i = i + 1;
        continue;

        return i;
}
