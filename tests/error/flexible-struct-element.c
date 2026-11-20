/* A struct with a flexible array member as the element of an array:
   6.7.3.2p3 keeps such a structure out of an array. */
struct buffer
{
        int length;
        char data[];
};

struct buffer buffers[4];

int
main(void)
{
        return 0;
}
