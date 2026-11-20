/* A flexible array member before an anonymous struct: only the last
   member may have an incomplete array type, 6.7.3.2p3, and an
   anonymous structure is a member, p15. */
struct buffer
{
        int length;
        char data[];
        struct
        {
                int tail;
        };
};

int
main(void)
{
        return 0;
}
