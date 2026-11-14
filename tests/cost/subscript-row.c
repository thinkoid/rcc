/* backend.md 6.3 and aarch64.md 6.3, &p[i]: a row of an array of
   arrays decays to its address, as the address of a subscript.  With a
   constant index of 0 that address is the bare base whatever the row's
   size, so on int m[2][3], whose rows of 12 bytes are no scale, m[0] is
   m and no instruction.  Any other constant index is the row of p + n.

   On x86-64 the listing must show first() returning the array's
   address, leaq -24(%rbp), %rax, with no instruction after it,
   element() reading m[0][1] by one movl 4(%rax), %eax after that leaq,
   and second() adding the row's 12 bytes after it, addq $12, %rax.

   On aarch64 the listing must show first() returning the array's
   address, add x0, x29, #16, with no instruction after it, element()
   reading m[0][1] by one ldr w0, [x0, #4] after that add, and second()
   adding the row's 12 bytes after it, add x0, x0, #12. */
int *first(void)
{
    int m[2][3];
    return m[0];
}

int element(void)
{
    int m[2][3];
    return m[0][1];
}

int *second(void)
{
    int m[2][3];
    return m[1];
}
