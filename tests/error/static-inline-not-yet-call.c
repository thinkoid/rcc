/* A call of a static inline function the program's own header defines
   with a body rcc does not compile yet, a floating constant: the
   function is kept uncompiled, and the call is refused by what its
   body needs.  The line markers say what clang -E says of an #include
   of the program's own header: the definition is the text of one,
   header.h, with no flag 3, and the program's own text follows it. */
# 1 "header.h" 1
static inline int
half(int x)
{
        return x * 0.5;
}
# 14 __BASE_FILE__ 2

int
main(void)
{
        return half(2);
}
