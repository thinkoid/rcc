/* backend.md 6.3, a < b and the rest as a value: cmp, setcc and
   movzbl.  The listing must show cmpl %ecx, %eax, setl %al and
   movzbl %al, %eax in less(), the same with sete in equal(), and no
   conditional jump. */
int less(int a, int b)
{
    return a < b;
}

int equal(int a, int b)
{
    return a == b;
}
