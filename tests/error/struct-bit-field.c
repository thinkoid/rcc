/* A struct with a bit-field, which rcc does not lay out yet: its size
   is refused by the bit-field's words. */
struct flags
{
        unsigned a : 1;
        unsigned b : 3;
};

int
main(void)
{
        return sizeof(struct flags);
}
