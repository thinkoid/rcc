/* A member access into a packed struct, which rcc does not lay out
   yet: refused by the attribute's words. */
struct flags
{
        int count;
        char dirty;
} __attribute__((packed));

int
first(struct flags *p)
{
        return p->count;
}

int
main(void)
{
        return 0;
}
