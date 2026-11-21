/* cost.md chapter 1, inline: a static inline function the program's
   own header defines, which nothing names and whose body c1 refuses,
   writes nothing, as one whose body the parser stops in: no symbol, no
   string literal and no static object of its own.  frame() takes two
   locals of 10 MB, a frame of 16 MiB or more on aarch64, and stride()
   steps through elements of 2 GiB, too large for an x86-64 immediate.
   The line markers say what clang -E says of an #include of the
   program's own header: the definitions are the text of one, header.h,
   with no flag 3, and the program's own text follows it.

   On aarch64 the listing is stride() and main(), and on x86-64
   frame() and main(), each with its static object; the literal of main
   is .Ls1 on each, after the one of the function written. */
# 1 "header.h" 1
static inline int
frame(void)
{
        static int calls;
        const char *name = "frame";
        char a[10000000];
        char b[10000000];

        ++calls;
        return a[0] + b[0] + name[0];
}

static inline int
stride(char a[][2147483648])
{
        static int calls;
        const char *name = "stride";

        ++calls;
        return a[1][0] + name[0];
}
# 37 __BASE_FILE__ 2

int
main(void)
{
        const char *name = "main";

        return name[0] - 'm';
}
