/* A union that holds a struct with a flexible array member as the
   element of an array: 6.7.3.2p3 keeps such a union out of an
   array. */
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

union either table[2];

int
main(void)
{
        return 0;
}
