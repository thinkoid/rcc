/* A pointer to long ordered against a pointer to long long, of one
   machine type and two C types: 6.5.9p2 asks pointers to compatible
   object types. */
int
main(void)
{
        long a = 1;
        long long b = 2;
        long *p = &a;
        long long *q = &b;

        return p < q;
}
