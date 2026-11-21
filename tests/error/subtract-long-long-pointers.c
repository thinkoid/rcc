/* The difference of a pointer to long and a pointer to long long, of
   one machine type and two C types: 6.5.7p3 asks pointers to
   compatible complete object types. */
int
main(void)
{
        long a = 1;
        long long b = 2;
        long *p = &a;
        long long *q = &b;

        return p - q;
}
