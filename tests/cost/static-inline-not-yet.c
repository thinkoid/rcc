/* cost.md chapter 1, inline: a static inline function the program's
   own header defines with a body rcc does not compile yet, half() and
   its floating constant, is kept uncompiled, and c1 writes nothing for
   it, so a file that never calls it compiles.  one() of the same
   header is compiled, and main() calls it.  The line markers say what
   clang -E says of an #include of the program's own header: the
   definitions are the text of one, header.h, with no flag 3, and the
   program's own text follows it.

   On each target the listing is one() and main() alone, with no
   symbol and no data of half(). */
# 1 "header.h" 1
static inline int
half(int x)
{
        static int calls;

        ++calls;
        return x * 0.5;
}

static inline int
one(void)
{
        return 1;
}
# 28 __BASE_FILE__ 2

int
main(void)
{
        return one();
}
