/* A block's typedef of a variable length array, whose size the typedef
   fixes, 6.7.9p8: rcc does not compile it yet. */
unsigned long
f(int n)
{
        typedef char row_t[n];

        return 0;
}

int
main(void)
{
        return f(1);
}
