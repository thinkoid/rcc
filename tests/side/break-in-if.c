/* A break as the then part of an if in no loop, 6.8.7.4p1. */
int
main(void)
{
        int a = 1;
        if (a)
                break;
        return 0;
}
