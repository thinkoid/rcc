/* main falls off its end after a while: the label past the loop is
   placed last, so a path reaches the end, and main returns 0 there,
   5.1.2.3.4. */
int
main(void)
{
        int a = 1;
        while (a)
                a = a - 1;
}
