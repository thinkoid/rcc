/* A static function called before its definition, which a system
   header writes static inline: rcc does not compile the definition, so
   the call, which would need it, is not yet, at the end of the unit.
   The line marker says what clang -E says of an #include of a system
   header: the definition is the text of one, header.h, by the flag
   3. */
static int helper(int x);

int
main(void)
{
        return helper(1);
}

# 1 "header.h" 1 3
static inline int
helper(int x)
{
        return x + 1;
}
