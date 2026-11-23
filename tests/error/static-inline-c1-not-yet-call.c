/* A call of a static inline function the program's own header defines
   with a body c1 refuses, two locals of 10 MB: the function is named,
   so it is not dropped, and the refusal stops the file as for any
   function, a frame past the limit on aarch64.  x86-64 compiles it.
   The line markers say what clang -E says of an #include of the
   program's own header: the definition is the text of one, header.h,
   with no flag 3, and the program's own text follows it. */
# 1 "header.h" 1
static inline int
frame(void)
{
        char a[10000000];
        char b[10000000];

        return a[0] + b[0];
}
# 18 __BASE_FILE__ 2

int
main(void)
{
        return frame();
}
