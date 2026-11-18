/* A struct with no members, which 6.7.3.2p10 leaves undefined and GNU C
   takes: rcc does not lay it out yet, and its size is refused. */
struct none
{
};

int
main(void)
{
        return sizeof(struct none);
}
