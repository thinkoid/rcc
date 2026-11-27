/* main after a for whose condition is omitted and whose body has no
   break: no label follows the jump back, which is the last instruction,
   so main writes no return of 0. */
int
main(void)
{
        int a = 1;
        for (;;)
                ;
}
