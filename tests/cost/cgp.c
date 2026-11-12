/* backend.md 7.2 and aarch64.md 7.2, if: an if with an empty body is
   still a test and a branch.

   On x86-64 the listing must show a loaded, compared with cmpl $0, %eax
   and a je to the label that follows at once, and only then the second
   if, a compared with cmpl $1, %eax and a jle past the store of 1 to
   x's slot.

   On aarch64 the listing must show a loaded and tested with cbz w0 to
   the label that follows at once, and only then the second if, a
   compared with cmp w0, #1 and a b.le past the store of 1 to x's slot,
   mov w0, #1 and str w0, [x29, #20]. */
int empty_if(int a)
{
    int x = 0;
    if (a) {
    }
    if (a > 1)
        x = 1;
    return x;
}
