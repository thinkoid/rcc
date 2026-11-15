/* A call of a static inline function a system header defines, as
   bits/byteswap.h defines __bswap_16: the definition is accepted and
   not compiled, and the call, which would need it compiled, is not
   yet.  The line markers say what clang -E says of an #include of a
   system header: the definition is the text of one, header.h, by the
   flag 3, and the program's own text follows it. */
# 1 "header.h" 1 3
static __inline unsigned short
swap(unsigned short x)
{
        return (x >> 8 & 0xff) | (x & 0xff) << 8;
}
# 14 __BASE_FILE__ 2

int
main(void)
{
        return swap(1);
}
