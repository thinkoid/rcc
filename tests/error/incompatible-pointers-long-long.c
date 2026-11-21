/* A pointer to long assigned to a pointer to long long: one machine
   type on LP64 and two C types, 6.2.5p4, so the pointees are not
   compatible and 6.5.17.2p1 asks a cast. */
int
main(void)
{
        long a = 1;
        long *p = &a;
        long long *q = p;

        return 0;
}
