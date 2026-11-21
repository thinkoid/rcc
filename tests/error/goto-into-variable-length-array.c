/* A goto from outside the scope of a variable length array to a label
   inside it, 6.8.7.2p1: the jump would pass the declaration that takes
   the array's storage. */
int
f(int n)
{
        goto in;
        {
                char buf[n];
        in:
                buf[0] = 1;
                return buf[0];
        }
}

int
main(void)
{
        return f(1);
}
