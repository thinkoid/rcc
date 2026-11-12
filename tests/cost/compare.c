/* backend.md 6.3 and aarch64.md 6.3, a < b and the rest as a value: a
   compare, and its condition written into the register as 1 or 0.

   On x86-64 the listing must show cmpl %ecx, %eax, setl %al and
   movzbl %al, %eax in less(), the same with sete in equal(), and no
   conditional jump.

   On aarch64 the listing must show cmp w0, w1 and cset w0, lt in
   less(), the same with cset w0, eq in equal(), and no conditional
   branch. */
int less(int a, int b)
{
    return a < b;
}

int equal(int a, int b)
{
    return a == b;
}
