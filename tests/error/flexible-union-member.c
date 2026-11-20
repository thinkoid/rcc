/* A union that holds a struct with a flexible array member, through
   another union, as a member of a struct: 6.7.3.2p3 keeps such a
   union, containing the structure possibly recursively, out of a
   structure.  A union may hold the struct, so the two unions are
   accepted. */
struct buffer
{
        int length;
        char data[];
};

union either
{
        int code;
        struct buffer buffer;
};

union any
{
        long word;
        union either either;
};

struct frame
{
        int kind;
        union any body;
};

int
main(void)
{
        return 0;
}
