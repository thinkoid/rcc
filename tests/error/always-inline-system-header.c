/* A function a system header defines always_inline, as stdbit.h
   defines each of its own: a system header's text is read as it is,
   so the attribute is dropped with no warning, the static inline body
   is skipped, and the program, which does not call the function,
   compiles with no word.  The line markers say what clang -E says of
   an #include of a system header: the function is the text of one,
   header.h, by the flag 3, and the program's own text follows it. */
# 1 "header.h" 1 3
static inline int __attribute__((always_inline))
step(int n)
{
        return n + 1;
}
# 14 __BASE_FILE__ 2

int
main(void)
{
        return 0;
}
