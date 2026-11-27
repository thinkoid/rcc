/* Arguments past the registers.  On x86-64 the seventh and later are
   pushed from their slots, last first, through rax, movl -40(%rbp),
   %eax and pushq %rax, after the last argument's store and before the
   loads of the first six, over a subq $8, %rsp when their count is
   odd; the call is followed by addq of what was pushed.  On aarch64
   the ninth and later are copied in order through w17 to an outgoing
   area opened below the frame for the call, 8 bytes each rounded up to
   16, ldr w17, [x29, #60] and str w17, [sp], and closed after the bl.
   A call inside an argument is complete before any push or copy, and
   opens and closes its own area. */
int g7(int, int, int, int, int, int, int);
int g8(int, int, int, int, int, int, int, int);
int g9(int, int, int, int, int, int, int, int, int);
int g10(int, int, int, int, int, int, int, int, int, int);
int g11(int, int, int, int, int, int, int, int, int, int, int);
int h(int);

int
seven(void)
{
        return g7(1, 2, 3, 4, 5, 6, 7);
}

int
eight(void)
{
        return g8(1, 2, 3, 4, 5, 6, 7, 8);
}

int
nine(void)
{
        return g9(1, 2, 3, 4, 5, 6, 7, 8, 9);
}

int
ten(void)
{
        return g10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
}

int
eleven(void)
{
        return g11(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
}

int
seven_nested(int a)
{
        return g7(a, a, a, a, a, a, h(a));
}

int
nine_in_nine(int a)
{
        return g9(a, a, a, a, a, a, a, a, g9(a, a, a, a, a, a, a, a, a));
}
