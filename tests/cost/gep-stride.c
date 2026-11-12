/* backend.md 6.3 and aarch64.md 6.3, m[i] as a pointer, of a size that
   is not a scale: the index multiplied by the size, then added.  m[i]
   on int m[2][3] is a row of 12 bytes, taken as a pointer.

   On x86-64 the listing must show the array's address from
   leaq -28(%rbp), %rax, the index widened with movslq %ecx, %rcx,
   scaled by imulq $12, %rcx, %rcx and added with addq %rcx, %rax. */
int *row(int i)
{
    int m[2][3];
    int *p;

    p = m[i];
    return p;
}
