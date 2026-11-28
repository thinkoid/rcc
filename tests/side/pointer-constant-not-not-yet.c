/* ! of a pointer constant, which rcc writes and rcc-v2 would fold, is
   not yet, at the !. */
int
f(void)
{
        return !(int *) 0;
}
