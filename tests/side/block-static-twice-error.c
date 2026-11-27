/* Two block's statics of one name in one block. */
int
f(void)
{
        static int s;
        static int s;
        return s;
}
