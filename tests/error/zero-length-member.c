/* A struct whose last member is an array of size zero, GNU C's
   spelling of a flexible array member: the program's own text takes it
   as one, 6.7.3.2p20, with a warning, since a constant size is greater
   than zero, 6.7.7.3p1. */
struct buffer
{
        int length;
        char data[0];
};

int
main(void)
{
        return 0;
}
