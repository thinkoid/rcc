/* backend.md 6.3, a constant: movabsq for a 64-bit constant no 32-bit
   immediate holds.  Two such constants 8 apart are each written where
   they are used.  The listing must show s loaded into rax,
   movabsq $81985529216486895, %rcx and addq %rcx, %rax; t loaded into
   rcx and added; then movabsq $81985529216486903, %rcx and
   addq %rcx, %rax: two movabsq, each after the operand it is added
   to, and the second not computed from the first. */
long two_constants(long s, long t)
{
    return s + 81985529216486895 + t + 81985529216486903;
}
