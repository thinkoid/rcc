/* static in the declaration of a for: rcc takes a local there, or a
   register variable. */
int
main(void)
{
        int sum = 0;

        for (static int i = 0; i < 3; i++)
                sum += i;

        return sum;
}
