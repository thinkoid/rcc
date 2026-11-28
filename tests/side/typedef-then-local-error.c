/* A typedef name and a local of one name in one block: T T is a
   redefinition where the typedef is of the same block. */
int
f(void)
{
        typedef int T;
        T T = 1;
        return T;
}
