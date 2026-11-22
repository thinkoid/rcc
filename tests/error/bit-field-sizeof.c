/* sizeof of a bit-field: sizeof applies to no expression that
   designates a bit-field member, 6.5.4.4p1. */
struct flags
{
        int count;
        unsigned dirty : 1;
};

int
main(void)
{
        struct flags f;

        return sizeof f.dirty;
}
