/* An array of size zero before another member: no flexible array
   member stands there, 6.7.3.2p3, so the size is refused as any
   array's, 6.7.7.3p1. */
struct buffer
{
        int length;
        char data[0];
        int tail;
};

int
main(void)
{
        return 0;
}
