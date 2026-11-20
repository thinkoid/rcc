/* A system header's struct with a flexible array member as a member
   of a struct and as the element of an array, as the kernel's
   netfilter headers write them: a system header is read as it is, so
   both are laid out, the member at its alignment and the array by the
   struct's size, with no word.  The line markers say what clang -E
   says of an #include of a system header: the records are the text of
   one, header.h, by the flag 3, and the program's own text follows
   it. */
# 1 "header.h" 1 3
struct entry
{
        int size;
        unsigned char elems[0];
};

struct standard
{
        struct entry entry;
        int verdict;
};

struct table
{
        int count;
        struct entry entries[];
};
# 28 __BASE_FILE__ 2

int
main(void)
{
        return sizeof(struct standard) + sizeof(struct table);
}
