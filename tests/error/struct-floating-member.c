/* A struct with a member of a floating type, which rcc does not use
   yet: its size is refused by the member type's words. */
struct sample
{
        int count;
        double mean;
};

int
main(void)
{
        return sizeof(struct sample);
}
