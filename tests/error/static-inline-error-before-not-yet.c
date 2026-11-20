/* An error in the body of a static inline function the program's own
   header defines, before what rcc does not compile yet: the error
   stays the error it is, since an identifier in an expression has a
   visible declaration, 6.5.2p2.  The line
   markers say what clang -E says of an #include of the program's own
   header: the definition is the text of one, header.h, with no flag 3,
   and the program's own text follows it. */
# 1 "header.h" 1
static inline int
half(int x)
{
        return undeclared + x * 0.5;
}
# 14 __BASE_FILE__ 2

int
main(void)
{
        return 0;
}
