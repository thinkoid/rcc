/* A flexible array member before a bit-field: only the last member may
   have an incomplete array type, 6.7.3.2p3, and a bit-field is a
   member. */
struct buffer
{
        int length;
        char data[];
        unsigned flags : 3;
};

int
main(void)
{
        return 0;
}
