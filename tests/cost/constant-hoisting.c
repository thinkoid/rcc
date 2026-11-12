/* backend.md 6.3 and aarch64.md 6.4, a constant no immediate holds:
   formed in a register where it is used.  Two such 64-bit constants 8
   apart are each written where they are used.

   On x86-64 the listing must show s loaded into rax,
   movabsq $81985529216486895, %rcx and addq %rcx, %rax; t loaded into
   rcx and added; then movabsq $81985529216486903, %rcx and
   addq %rcx, %rax: two movabsq, each after the operand it is added to,
   and the second not computed from the first.

   On aarch64 the listing must show s loaded into x0, the first
   constant, 0x0123456789abcdef, formed in x1 by mov x1, #52719 and a
   movk for each of its other three 16-bit parts, and add x0, x0, x1; t
   loaded into x1 and added; then the second formed the same way from
   mov x1, #52727, and the add: two sequences of four, each after the
   operand it is added to, and the second not computed from the
   first. */
long two_constants(long s, long t)
{
    return s + 81985529216486895 + t + 81985529216486903;
}
