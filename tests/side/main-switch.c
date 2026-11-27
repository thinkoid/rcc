/* main whose switch has a default and no break ends terminated: no
   return of 0 follows it. */
int
main(void)
{
        int a = 1;
        switch (a) {
        case 1: return 1;
        default: return 2;
        }
}
