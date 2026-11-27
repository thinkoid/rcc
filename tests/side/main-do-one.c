/* main after do ... while (1): the test is a conditional jump, so a
   path reaches main's end, and main returns 0 there. */
int
main(void)
{
        int a = 1;
        do
                a = 1;
        while (1);
}
