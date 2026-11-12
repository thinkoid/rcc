/* backend.md chapter 8 and aarch64.md chapter 8, arguments past the
   convention's registers: each argument is stored to its slot as it is
   evaluated, and those the registers do not take are placed on the
   stack from their slots.  seven() passes seven ints and eight() eight.

   On x86-64 the listing must show each constant argument stored to its
   slot in one instruction, movl $1, -4(%rbp) and on; seven() with one
   stack argument over an 8-byte pad, subq $8, %rsp, the seventh pushed,
   the first six loaded into edi, esi, edx, ecx, r8d and r9d, the call
   and addq $16, %rsp; and eight() with two stack arguments and no pad,
   the eighth pushed before the seventh, and addq $16, %rsp.  An int
   slot is 4 bytes, so each stack argument is loaded into eax and pushed
   from rax.

   On aarch64, whose convention takes eight arguments in registers, the
   listing is each constant argument stored through w0, mov w0, #1 and
   str w0, [x29, #16] and on, and every argument loaded from its slot
   into w0 to w6 in seven() and w0 to w7 in eight(), with no outgoing
   area; the row is not reached there, and stack-args-nine.c reaches
   it. */
int f7(int, int, int, int, int, int, int);
int f8(int, int, int, int, int, int, int, int);

int seven(void)
{
    return f7(1, 2, 3, 4, 5, 6, 7);
}

int eight(void)
{
    return f8(1, 2, 3, 4, 5, 6, 7, 8);
}
