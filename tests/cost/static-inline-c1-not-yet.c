/* cost.md chapter 1, inline: a static inline function the program's
   own header defines, which nothing names and whose body c1 refuses,
   writes nothing, as one whose body the parser stops in: no symbol, no
   string literal and no static object of its own.  stride() steps
   through elements of 2 GiB, too large for an x86-64 immediate.  A
   frame past the target's limit is no such refusal: it is an error in
   any function (shape.md chapter 7).  The line markers say what clang
   -E says of an #include of the program's own header: the definition
   is the text of one, header.h, with no flag 3, and the program's own
   text follows it.

   On x86-64 the listing is main() alone, its literal .Ls0.  The
   aarch64 generator refuses no construct a program of this size can
   hold, and the listing there is stride(), with its static object, and
   main(), whose literal is .Ls1, after stride()'s. */
# 1 "header.h" 1
static inline int
stride(char a[][2147483648])
{
        static int calls;
        const char *name = "stride";

        ++calls;
        return a[1][0] + name[0];
}
# 27 __BASE_FILE__ 2

int
main(void)
{
        const char *name = "main";

        return name[0] - 'm';
}
