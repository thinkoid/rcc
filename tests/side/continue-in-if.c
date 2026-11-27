/* A continue as the then part of an if in no loop, 6.8.7.3p1. */
int
main(void)
{
        int a = 1;
        if (a)
                continue;
        return 0;
}
