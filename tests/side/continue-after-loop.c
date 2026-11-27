/* A continue after a loop's body is in no loop, 6.8.7.3p1. */
int
main(void)
{
        for (;;) {
        }
        continue;
}
