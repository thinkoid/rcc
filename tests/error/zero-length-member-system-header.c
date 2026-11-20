/* A struct a system header defines with an array of size zero as its
   last member, as glibc's fcntl.h writes struct file_handle: a system
   header is read as it is, so the member is a flexible array member,
   6.7.3.2p20, with no warning.  The line markers say what clang -E
   says of an #include of a system header: the struct is the text of
   one, header.h, by the flag 3, and the program's own text follows
   it. */
# 1 "header.h" 1 3
struct handle
{
        unsigned int bytes;
        unsigned char data[0];
};
# 15 __BASE_FILE__ 2

int
main(void)
{
        return 0;
}
