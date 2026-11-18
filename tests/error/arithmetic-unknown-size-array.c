/* Arithmetic on a pointer to an array of unknown size: the array is
   incomplete, 6.2.5p27, and an addition takes a pointer to a complete
   object type, 6.5.7p2. */
extern int a[];

long
f(void)
{
        return (long) (&a + 1);
}

int
main(void)
{
        return 0;
}
