/* A break after a loop's body is in no loop, 6.8.7.4p1. */
int
main(void)
{
        while (1)
                ;
        break;
}
