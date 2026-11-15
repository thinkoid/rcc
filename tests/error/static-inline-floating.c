/* A floating constant in the body of a static inline function, as
   clang's tgmath.h writes 0.F: the lexer takes the constant, the body
   is skipped, and the call of the function is not yet. */
static __inline int
half(int x)
{
        return x * 0.5F;
}

int
main(void)
{
        return half(2);
}
