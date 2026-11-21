/* A call of a static inline function the program's own header defines
   with a head rcc does not compile yet, a struct returned by value:
   the function is kept uncompiled, and the call is refused by what its
   head needs.  The line markers say what clang -E says of an #include
   of the program's own header: the definition is the text of one,
   header.h, with no flag 3, and the program's own text follows it. */
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
# 21 __BASE_FILE__ 2

int
main(void)
{
        return pair(1, 2).a;
}
