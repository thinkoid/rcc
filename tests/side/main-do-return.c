/* main after a do whose body returns: no test is written and no label
   placed after it, so the return's jump is the last instruction and
   main writes no return of 0. */
int
main(void)
{
        int a = 1;
        do {
                return 1;
        } while (a);
}
