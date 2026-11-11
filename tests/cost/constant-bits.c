/* backend.md 6.4 and aarch64.md 6.4, a constant operand: the immediate
   of the instruction where it has an encoding for the value, else the
   constant held in a register first, in as many instructions as its
   bits need.  The functions add, and, and add at 64 bits, constants
   chosen by their bits.

   On x86-64 the listing must show every 32-bit constant as the
   immediate of its instruction, addl $5000, %eax and andl $5, %eax
   among them, -100000L as addq $-100000, %rax, and the two 64-bit
   constants no 32-bit immediate holds loaded into rcx with movabsq.

   On aarch64 the listing must show, in add_bits(), 4095 and 4096 as
   add's immediate, no instruction; 5000, -5000 and the bitmask
   0x80000001 held in w1 by one mov each, mov w1, #5000, mov w1,
   #-5000 and mov w1, #0x80000001; and 0x80000002 by two, mov w1, #2
   and movk w1, #0x8000, lsl #16.  In and_bits(), 0xff as and's
   immediate, and 5, no bitmask, held by mov w1, #5.  In long_bits(),
   -100000L by two from ones, mov x1, #-34464 and movk x1, #0xfffe,
   lsl #16; 0x0000ffff00000000 by one, mov x1, #281470681743360; and
   the bitmask 0xffff0000ffff0000 by one, mov x1,
   #0xffff0000ffff0000. */
unsigned add_bits(unsigned x)
{
    return x + 4095 + 4096 + 5000 + -5000 + 0x80000001 + 0x80000002;
}

unsigned and_bits(unsigned x)
{
    return x & 0xff & 5;
}

long long_bits(long x)
{
    return x + -100000L + 0x0000ffff00000000 + 0xffff0000ffff0000;
}
