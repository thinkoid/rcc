/* A string literal as a pointer's initializer: the array decays to an
   address constant, 6.6p11, not yet. */
char *greeting = "hello";

int
main(void)
{
        return greeting[0];
}
