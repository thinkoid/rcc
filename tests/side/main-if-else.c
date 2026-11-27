/* main ends in an if whose two parts return: the label past the then
   part is followed by the else part's jump, the last instruction, so
   no path reaches main's end and no return of 0 is written. */
int
main(void)
{
        int a = 1;
        if (a)
                return 1;
        else
                return 2;
}
