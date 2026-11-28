/* &a points to the array, &a[0] to its element: two distinct pointer
   types, 6.5.10p2. */
int
f(void)
{
        int a[3];
        return &a[0] == &a;
}
