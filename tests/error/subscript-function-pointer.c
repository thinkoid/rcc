/* A subscript of a pointer to a function: the pointer of [] points to
   an object type, 6.5.3.2p1. */
int
f(int (*g)(void))
{
        return g[0] != 0;
}

int
main(void)
{
        return 0;
}
