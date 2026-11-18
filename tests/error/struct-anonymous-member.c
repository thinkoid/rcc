/* A struct with an anonymous union, 6.7.3.2p15, which rcc does not lay
   out yet: its size is refused by the member's words. */
struct value
{
        int kind;
        union
        {
                int i;
                long l;
        };
};

int
main(void)
{
        return sizeof(struct value);
}
