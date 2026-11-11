/* backend.md chapter 5 and aarch64.md 5.2, the frame's size: the
   prologue subtracts the frame from the stack pointer, and the
   epilogue adds it back, in the immediate of the instruction where it
   encodes the size.  large()'s frame is 8208 bytes on aarch64, and
   aligned()'s 8192, a multiple of 4096.

   On x86-64 the listing is one subq in each function, $8192 and
   $8176, since the immediate of sub takes 32 bits; the row is not
   reached there.

   On aarch64 the listing must show large()'s frame opened in two
   instructions, sub sp, sp, #8192 and then sub sp, sp, #16, the upper
   part first, each followed by its .cfi_def_cfa_offset, and closed by
   two adds in the same order; and aligned()'s opened by one, sub sp,
   sp, #8192, its low 12 bits being zero, and closed by one add. */
int large(int a)
{
    char pad[8180];

    return a;
}

int aligned(int a)
{
    char pad[8172];

    return a;
}
