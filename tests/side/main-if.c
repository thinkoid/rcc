/* main falls off its end after an if without an else: the label past
   the then part is placed last, so a path reaches the end, and main
   returns 0 there, 5.1.2.3.4. */
int
main(void)
{
        int a = 1;
        if (a)
                return 1;
}
