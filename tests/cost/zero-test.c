/* backend.md 7.1 and aarch64.md 7.1, the constant 0 as a condition: the
   code for a constant condition is the code for a condition; nothing
   looks at the value.  In not_zero(), the parser folds the constant
   expression !0 to 1 (backend.md 6.4 and aarch64.md 6.4).

   On x86-64 the listing of never() must show movl $0, %eax,
   cmpl $0, %eax and a je past the body of the if, the body present, and
   each return jumping to the epilogue label; and of not_zero(),
   movl $1, %eax and no cmp.

   On aarch64 the listing of never() must show mov w0, #0 and cbz w0
   past the body of the if, the body present, and each return branching
   to the epilogue label; and of not_zero(), mov w0, #1 and no cmp. */
int never(void)
{
    if (0)
        return 1;
    return 2;
}

int not_zero(void)
{
    return !0;
}
