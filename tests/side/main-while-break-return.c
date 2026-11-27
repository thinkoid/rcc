/* main after a while whose body breaks or returns: the label past the
   loop is placed last, and main returns 0 there. */
int
main(void)
{
        int a = 1;
        int g = 2;
        while (g) {
                if (a)
                        break;
                else
                        return 3;
        }
}
