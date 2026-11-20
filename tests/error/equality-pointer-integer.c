/* A pointer compared for equality with an integer other than a null
   pointer constant. */
int
main(void)
{
        int a = 1;
        int *p = &a;

        return p == 1;
}
