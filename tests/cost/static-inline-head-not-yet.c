/* cost.md chapter 1, inline: a static inline function the program's
   own header defines with a head rcc does not compile yet is kept
   uncompiled, as for a body, and c1 writes nothing for it, so a file
   that never calls it compiles.  pair() returns a struct by value,
   first() takes one, and half() takes a floating parameter.  The line
   markers say what clang -E says of an #include of the program's own
   header: the definitions are the text of one, header.h, with no
   flag 3, and the program's own text follows it.

   On each target the listing is main() alone, with no symbol of the
   three. */
# 1 "header.h" 1
struct pair
{
        int a, b;
};

static inline struct pair
pair(int a, int b)
{
        struct pair p = { a, b };

        return p;
}

static inline int
first(struct pair p)
{
        return p.a;
}

static inline int
half(double x)
{
        return x / 2;
}
# 38 __BASE_FILE__ 2

int
main(void)
{
        return 0;
}
