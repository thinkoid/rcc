/* An initializer for a flexible array member: ISO C gives the member
   no object of its own, 6.7.3.2p20, and GNU C takes the list for an
   object of static storage duration, which rcc does not take yet.  The
   value of the member before it is taken, and the refusal is at the
   member's own initializer. */
struct buffer
{
        int length;
        char data[];
};

struct buffer b = { 2, { 'a', 'b' } };

int
main(void)
{
        return b.length;
}
