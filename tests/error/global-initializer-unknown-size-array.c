/* An address constant that steps a pointer to an array of unknown
   size, which has no size to step by: the addition takes a pointer to
   a complete object type, 6.5.7p2. */
extern int a[];

int (*q)[] = &a + 1;

int
main(void)
{
        return 0;
}
