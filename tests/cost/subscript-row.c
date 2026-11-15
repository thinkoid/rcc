/* backend.md 6.3 and aarch64.md 6.3, &p[i]: a row of an array of
   arrays decays to its address, as the address of a subscript.  On a
   local array a row by a constant index is the address by a name and a
   constant, the array's slot plus the row's offset, whatever the row's
   size: on int m[2][3], whose rows of 12 bytes are no scale, m[0] is
   m's slot, m[1] the slot 12 bytes up, and m[0][1] one access at the
   slot 4 bytes up.

   On x86-64 the listing must show first() returning the array's
   address, leaq -24(%rbp), %rax, with no instruction after it,
   element() reading m[0][1] by the one movl -20(%rbp), %eax, and
   second() forming the row's address by the one leaq -12(%rbp), %rax.

   On aarch64 the listing must show first() returning the array's
   address, add x0, x29, #16, with no instruction after it, element()
   reading m[0][1] by the one ldr w0, [x29, #20], and second() forming
   the row's address by the one add x0, x29, #28. */
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
