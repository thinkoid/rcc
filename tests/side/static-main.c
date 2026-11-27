/* main defined static, which would hide it from the startup code,
   5.1.2.3.2p1: the error is at the static. */
static int
main(void)
{
        return 0;
}
