/* A conditional expression whose arms point to long and to long long,
   of one machine type and two C types: 6.5.16p3 asks pointers to
   compatible types. */
int
main(void)
{
        long a = 1;
        long long b = 2;
        long *p = &a;
        long long *q = &b;

        return *(1 ? p : q) != 1;
}
