/* A struct with a flexible array member as a member of a struct:
   6.7.3.2p3 keeps such a structure out of a structure, wherever the
   member stands. */
struct buffer
{
        int length;
        char data[];
};

struct frame
{
        int kind;
        struct buffer payload;
};

int
main(void)
{
        return 0;
}
