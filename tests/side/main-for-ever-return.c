/* main after a for whose condition is omitted and whose body returns:
   the if's label is placed before the jump back, the last
   instruction, and main writes no return of 0. */
int
main(void)
{
        int a = 1;
        for (;;) {
                if (a)
                        return 1;
        }
}
