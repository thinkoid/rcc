/* An initializer of a bit-field in a local's list, which rcc does not
   store yet: refused at the value. */
struct flags
{
        int count;
        unsigned dirty : 1;
};

int
main(void)
{
        struct flags local = { 1, 1 };

        return local.count;
}
