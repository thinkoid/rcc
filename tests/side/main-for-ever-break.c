/* main after a for whose condition is omitted and whose body breaks:
   the break names the label past the loop, which is placed last, and
   main returns 0 there. */
int
main(void)
{
        int a = 1;
        for (;;) {
                if (a)
                        break;
        }
}
