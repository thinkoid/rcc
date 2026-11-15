/* A call of a static inline function, as bits/byteswap.h defines
   __bswap_16: the definition is accepted and not compiled, and the
   call, which would need it compiled, is not yet. */
static __inline unsigned short
swap(unsigned short x)
{
        return (x >> 8 & 0xff) | (x & 0xff) << 8;
}

int
main(void)
{
        return swap(1);
}
