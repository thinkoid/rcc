/* A member an attribute aligns, which changes the struct's layout, as
   rcc does not honour yet: its size is refused by the attribute's
   words. */
struct block
{
        char tag;
        long data __attribute__((aligned(16)));
};

int
main(void)
{
        return sizeof(struct block);
}
