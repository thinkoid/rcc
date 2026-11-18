/* A subscript of two pointers: one operand of [] is a pointer and the
   other an integer, 6.5.3.2p1. */
int
f(int *p, int *q)
{
        return p[q];
}

int
main(void)
{
        return 0;
}
