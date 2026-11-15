/* backend.md 7.3 and aarch64.md 7.3, the simple form.  The cases are
   written as 10, 0, -1 and 100: ncase is 4, range, 100 less -1, is 101,
   above 3 * 4 = 12, and there are fewer than ten cases.  A case's test
   for each, in the order of the values, -1, 0, 10, 100, then the jump to
   the default.

   On x86-64 the listing must show cmpl $-1, %eax and je .Lkind.4,
   cmpl $0, %eax and je .Lkind.3, cmpl $10, %eax and je .Lkind.2,
   cmpl $100, %eax and je .Lkind.5, then jmp .Lkind.6 to the default;
   then the body, the labels in the order written.

   On aarch64 the listing must show cmp w0, #-1 and b.eq .Lkind.4; the
   test of zero, cbz w0, .Lkind.3, for the 0; cmp w0, #10 and
   b.eq .Lkind.2; cmp w0, #100 and b.eq .Lkind.5; then b .Lkind.6. */
int
kind(int n)
{
        switch (n) {
        case 10: return 1;
        case 0: return 2;
        case -1: return 3;
        case 100: return 4;
        default: return 5;
        }
}
