/* A struct an attribute packs, which changes its layout, as rcc does
   not honour yet: its size is refused by the attribute's words. */
struct __attribute__((packed)) header
{
        char tag;
        int length;
};

int
main(void)
{
        return sizeof(struct header);
}
