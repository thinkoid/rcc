/* A break as the else part of an if in no loop, 6.8.7.4p1. */
int
main(void)
{
        int a = 1;
        if (a)
                ;
        else
                break;
        return 0;
}
