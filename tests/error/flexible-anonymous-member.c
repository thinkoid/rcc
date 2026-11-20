/* An anonymous struct with a flexible array member as a member of a
   struct: an anonymous structure is a member, 6.7.3.2p15, and p3 keeps
   a structure with a flexible array member out of a structure. */
struct frame
{
        int kind;
        struct
        {
                int length;
                char data[];
        };
};

int
main(void)
{
        return 0;
}
