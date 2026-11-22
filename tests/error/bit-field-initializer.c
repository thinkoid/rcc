/* An initializer of a bit-field in an object of static storage, which
   rcc does not fold yet: refused at the value.  The unnamed bit-field
   before it takes no initializer, 6.7.11p10, so the walk passes over
   it. */
struct flags
{
        int count;
        int : 3;
        unsigned dirty : 1;
};

struct flags global = { 1, 1 };

int
main(void)
{
        return 0;
}
