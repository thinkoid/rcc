/* backend.md 7.3 and aarch64.md 7.3, the direct form.  In day() the
   cases are 1, 2, 3 and 5: ncase is 4 and range, 5 less 1, is 4, above
   0 and at most 3 * 4 = 12.  The value less the first is compared
   unsigned with the range, above it to the default, and the jump goes
   through a table of range + 1 = 5 addresses, the fourth, for 4, the
   default's.  In digit() the cases are 0 to 3: ncase 4, range 3, and a
   first value of 0 is subtracted by nothing.

   On x86-64 the listing must show, in day(), subl $1, %eax, then
   cmpl $4, %eax and ja .Lday.6 to the default; leaq .Lday.8(%rip),
   %rcx and jmp *(%rcx,%rax,8), with no movl %eax, %eax before them,
   since the subl wrote the index at 32 bits; then .data, .p2align 3,
   .Lday.8, five .quad, the fourth .Lday.6, and .text; then the body,
   each case's label where it is written.  In digit(), cmpl $3, %eax and
   ja .Ldigit.6, then the width rule's movl %eax, %eax before the leaq,
   since the int was loaded valid to 32 bits and the index is read to
   64.

   On aarch64 the listing must show, in day(), sub w0, w0, #1,
   cmp w0, #4 and b.hi .Lday.6; adrp x16, .Lday.8,
   add x16, x16, :lo12:.Lday.8, ldr x16, [x16, w0, uxtw #3] and br x16;
   then the same table.  In digit(), cmp w0, #3, b.hi .Ldigit.6 and the
   same four instructions, the uxtw reading the index's 32 bits. */
int
day(int n)
{
        switch (n) {
        case 1: return 10;
        case 2: return 20;
        case 3: return 30;
        case 5: return 50;
        default: return 0;
        }
}

int
digit(int c)
{
        switch (c) {
        case 0: return 7;
        case 1: return 6;
        case 2: return 5;
        case 3: return 4;
        }

        return -1;
}
