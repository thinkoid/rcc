/* A flexible array member before another member: only the last member
   may have an incomplete array type, 6.7.3.2p3. */
struct buffer
{
        int length;
        char data[];
        int tail;
};

int
main(void)
{
        return 0;
}
