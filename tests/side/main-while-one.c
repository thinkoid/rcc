/* main after while (1), whose constant is tested: the label past the
   loop is placed, so main returns 0 there, though no path reaches it
   at run time. */
int
main(void)
{
        while (1)
                ;
}
