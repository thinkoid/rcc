/* The address of a variable length array, a pointer to one: rcc does
   not compile it yet. */
void *
f(int n)
{
        char buf[n];

        return &buf;
}

int
main(void)
{
        f(1);
        return 0;
}
