/* typedef in the declaration of a for, rcc's error. */
int
f(void)
{
        for (typedef int T;;)
                return 0;
}
