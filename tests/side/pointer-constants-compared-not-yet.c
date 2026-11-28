/* Two pointer constants compared, which rcc writes and rcc-v2 would
   fold, are not yet, at the operator. */
int
f(void)
{
        return (int *) 0 == 0;
}
