/* An unsigned constant size is its value, read with no sign. */
unsigned long
f(void)
{
        int a[3u];
        int b[0x2];
        return sizeof a + sizeof b;
}
