/* backend.md 6.3 and aarch64.md 6.3, -a and ~a, and the width rule.
   The negation and the complement write the value at its own width,
   and the value is valid to what that write left.

   On x86-64 the listing must show neg() negating a by negl %eax and
   complement() complementing it by notl %eax, after the load of a;
   widened() negating (unsigned) a by negl %eax with no instruction
   between it and the jmp to the epilogue, since the 32-bit write
   cleared the upper half and unsigned to unsigned long writes nothing;
   and signed_() negating a by negl %eax and widening the result to
   long with movslq %eax, %rax.

   On aarch64 the listing must show neg() negating a by neg w0, w0 and
   complement() complementing it by mvn w0, w0, after the load of a;
   widened() negating (unsigned) a by neg w0, w0 with no instruction
   between it and the branch to the epilogue; and signed_() negating a
   by neg w0, w0 and widening the result to long with sxtw x0, w0. */
int neg(int a)
{
    return -a;
}

int complement(int a)
{
    return ~a;
}

unsigned long widened(int a)
{
    return -(unsigned) a;
}

long signed_(int a)
{
    return -a;
}
