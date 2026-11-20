/* A function declared in the declaration of a for: the declaration
   declares objects of storage class auto or register alone, C17's
   6.8.5p3, so the function is an error, as static is there. */
int
main(void)
{
        for (int next(void);;)
                return 0;
}
