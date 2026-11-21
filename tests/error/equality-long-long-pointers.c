/* A pointer to long compared for equality with a pointer to long long,
   of one machine type and two C types: 6.5.10p2 asks pointers to
   compatible types. */
int
main(void)
{
        long a = 1;
        long long b = 2;
        long *p = &a;
        long long *q = &b;

        return p == q;
}
