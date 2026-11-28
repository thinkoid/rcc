/* The address of an int initializing a pointer to a pointer is an
   error at the =. */
int
f(int x)
{
        int **pp = &x;
        return 0;
}
