/* main whose switch has no default: the label past its body is placed,
   and the return of 0 follows it. */
int
main(void)
{
        int a = 1;
        switch (a) {
        case 1: return 1;
        case 2: return 2;
        }
}
