/* A local and a constant of one name in one block. */
int
f(void)
{
        int A = 1;
        enum { A };
        return A;
}
