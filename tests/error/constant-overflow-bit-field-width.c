/* A bit-field whose width overflows int on the way: 6.6p4 makes a
   constant expression out of its type's range a constraint violation,
   so rcc warns, and the width is the wrapped value, 3. */
struct s
{
        int f : (65536 * 65536) + 3;
};

int
main(void)
{
        struct s x = { 3 };

        return x.f;
}
