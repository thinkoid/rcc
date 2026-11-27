/* main falls off its end after an if whose then part is a block that
   returns: the label past it is placed last, and main returns 0 there,
   as for a then part that is the return alone. */
int
main(void)
{
        int a = 1;
        if (a) {
                return 1;
        }
}
