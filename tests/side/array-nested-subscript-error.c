/* An element of an array of int is no array: its subscript is the
   error. */
int
f(void)
{
        int a[3];
        return a[0][0];
}
