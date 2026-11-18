/* A struct with a flexible array member, 6.7.3.2p20, which rcc does not
   lay out yet: its size is refused by the member's words. */
struct buffer
{
        int length;
        char data[];
};

int
main(void)
{
        return sizeof(struct buffer);
}
