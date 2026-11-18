/* backend.md 6.3 and aarch64.md 6.3, &p[i]: a row of an array of
   arrays decays to its address, as the address of a subscript.  On a
   local array a row by a constant index is the address by a name and a
   constant, the array's slot plus the row's offset, whatever the row's
   size: on int m[2][3], whose rows of 12 bytes are no scale, m[0] is
   m's slot, m[1] the slot 12 bytes up, and m[0][1] one access at the
   slot 4 bytes up.  Through a pointer a row by a constant index is the
   pointer's value plus the row's offset, whatever the row's size, as an
   element's is: p[1] on int (*p)[3] is p plus 12.  An offset no 32-bit
   displacement holds is the sum of p + n, the 2400000000 bytes of
   p[200000000] held in a register and added.

   On x86-64 the listing must show first() returning the array's
   address, leaq -24(%rbp), %rax, with no instruction after it,
   element() reading m[0][1] by the one movl -20(%rbp), %eax, second()
   forming the row's address by the one leaq -12(%rbp), %rax, through()
   forming p[1]'s by the one leaq 12(%rax), %rax after p's load, and
   beyond() forming p[200000000]'s by movabsq $2400000000, %rcx and
   addq %rcx, %rax after p's load.

   On aarch64 the listing must show first() returning the array's
   address, add x0, x29, #16, with no instruction after it, element()
   reading m[0][1] by the one ldr w0, [x29, #20], second() forming the
   row's address by the one add x0, x29, #28, through() forming p[1]'s
   by the one add x0, x0, #12 after p's load, and beyond() forming
   p[200000000]'s by mov x1, #6144, movk x1, #0x8f0d, lsl #16 and
   add x0, x0, x1 after p's load. */
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

int *
through(int (*p)[3])
{
        return p[1];
}

int *
beyond(int (*p)[3])
{
        return p[200000000];
}
