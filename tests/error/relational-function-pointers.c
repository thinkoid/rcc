/* A relational operator on two pointers to a function: 6.5.9p2 takes
   two pointers to compatible object types, and a function is not an
   object.  == and != take them. */
int
less(int (*p)(void), int (*q)(void))
{
        return p == q || p < q;
}

int
main(void)
{
        return 0;
}
