/* main whose switch's break names the label past its body, which is
   placed, and the return of 0 follows it. */
int
main(void)
{
        int a = 1;
        switch (a) {
        case 1: break;
        default: return 2;
        }
}
