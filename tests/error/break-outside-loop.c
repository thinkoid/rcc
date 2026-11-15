/* A break outside a loop and a switch, 6.8.7.4p1: an error. */
int
main(void)
{
        if (1)
                break;

        return 0;
}
