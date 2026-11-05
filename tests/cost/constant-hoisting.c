/* Chapter 1, no moving or merging; chapter 2,
   -disable-constant-hoisting.  Two 64-bit constants 8 apart are each
   materialized where they are written.  The listing must show two
   movabs, each after the load of the operand it is added to;
   constant hoisting would materialize the first once at entry and
   make the second an add of 8 to it. */
long two_constants(long s, long t)
{
    return s + 81985529216486895 + t + 81985529216486903;
}
