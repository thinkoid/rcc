/* A floating constant in the body of a static inline function a system
   header defines, as clang's tgmath.h writes 0.F: the lexer takes the
   constant, the body is skipped, and the call of the function is not
   yet.  The line markers say what clang -E says of an #include of a
   system header: the definition is the text of one, header.h, by the
   flag 3, and the program's own text follows it. */
# 1 "header.h" 1 3
static __inline int
half(int x)
{
        return x * 0.5F;
}
# 14 __BASE_FILE__ 2

int
main(void)
{
        return half(2);
}
