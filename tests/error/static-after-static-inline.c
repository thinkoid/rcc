/* A static function the program defines after a system header's
   static inline definition of it, as a program that defines its own
   __bswap_16 after bits/byteswap.h would: the header's body is
   skipped, and still the function has a definition, so the program's
   is a second one.  The line markers say what clang -E says of an
   #include of a system header: the first definition is the text of
   one, header.h, by the flag 3, and the program's own text follows
   it. */
# 1 "header.h" 1 3
static __inline int
twice(int x)
{
        return 2 * x;
}
# 16 __BASE_FILE__ 2

static int
twice(int x)
{
        return x + x;
}

int
main(void)
{
        return 0;
}
