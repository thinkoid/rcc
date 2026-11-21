/* A block's static variable length array: an object of static storage
   duration has no variable length array type, 6.7.7.3p2. */
void
f(int n)
{
        static char buf[n];
}

int
main(void)
{
        f(1);
        return 0;
}
