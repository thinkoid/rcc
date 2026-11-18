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
   p[200000000] held in a register and added.  A variable index into
   the row joins that address on x86-64: p[1][i] is one access, the
   row's offset its displacement, and i[p[1]] the same, the index
   generated first, as written.  A further constant over the row joins
   the displacement, and where the two constants' sum does not fit 32
   bits the row's address is formed and the further constant is the
   displacement, (p[1] + 536870911)[i].  A row whose offset no 32-bit
   displacement holds is the sum of p + n, the access's base,
   p[200000000][i], and an element of a size no addressing form scales
   by, the row p[1][i] of 12 bytes on int (*p)[2][3], is the sum of
   p + n on the row's address.  The row plus a variable index as a
   value, p[1] + i, is the same address and one leaq of it on x86-64,
   returned, i + p[1] with the index generated first, into a register
   variable, and stored; at a size no addressing form scales by, the
   12 bytes of p[1] + i on int (*p)[2][3], it is the sum of p + n.

   On x86-64 the listing must show first() returning the array's
   address, leaq -24(%rbp), %rax, with no instruction after it,
   element() reading m[0][1] by the one movl -20(%rbp), %eax, second()
   forming the row's address by the one leaq -12(%rbp), %rax, through()
   forming p[1]'s by the one leaq 12(%rax), %rax after p's load,
   beyond() forming p[200000000]'s by movabsq $2400000000, %rcx and
   addq %rcx, %rax after p's load, column() reading p[1][i] by the one
   movl 12(%rax,%rcx,4), %eax after p's load and i's, with no leaq of
   the row's address, column_first() reading i[p[1]] by the one movl
   12(%rcx,%rax,4), %eax after i's load and p's, column_beyond() the
   movabsq and the addq, then movl (%rax,%rcx,4), %eax, column_sum()
   forming p[1]'s address by leaq 12(%rax), %rax and reading by movl
   2147483644(%rax,%rcx,4), %eax, and column_wide() forming p[1]'s
   address by leaq 24(%rax), %rax, then imulq $12, %rcx, %rcx and addq
   %rcx, %rax.  row_sum() forms p[1] + i by the one leaq
   12(%rax,%rcx,4), %rax after p's load and i's, row_sum_first() by
   leaq 12(%rcx,%rax,4), %rax after i's load and p's,
   row_sum_register() by leaq 12(%rax,%rcx,4), %rbx into the variable,
   row_sum_stored() by leaq 12(%rcx,%rdx,4), %rcx before the store
   movq %rcx, (%rax), and row_sum_wide() as column_wide(), leaq
   24(%rax), %rax, then imulq $12, %rcx, %rcx and addq %rcx, %rax.

   On aarch64 the listing must show first() returning the array's
   address, add x0, x29, #16, with no instruction after it, element()
   reading m[0][1] by the one ldr w0, [x29, #20], second() forming the
   row's address by the one add x0, x29, #28, through() forming p[1]'s
   by the one add x0, x0, #12 after p's load, beyond() forming
   p[200000000]'s by mov x1, #6144, movk x1, #0x8f0d, lsl #16 and
   add x0, x0, x1 after p's load, column() forming p[1]'s address by
   add x0, x0, #12 before ldr w0, [x0, w1, sxtw #2], since no form
   holds an index with a displacement, column_first() the same after
   i's load, column_beyond() and column_sum() forming the row's
   address and its sum in x0 before the access, and column_wide()
   forming p[1]'s address by add x0, x0, #24, then mov x2, #12, mul x1,
   x1, x2 and add x0, x0, x1.  row_sum(), row_sum_first(),
   row_sum_register() and row_sum_stored() form p[1]'s address by
   add x0, x0, #12, or x1 after i's load, then add the index by add
   x0, x0, w1, sxtw #2, the last add writing x19 in
   row_sum_register(), and row_sum_wide() as column_wide(). */
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

int
column(int (*p)[3], int i)
{
        return p[1][i];
}

int
column_first(int (*p)[3], int i)
{
        return i[p[1]];
}

int
column_beyond(int (*p)[3], int i)
{
        return p[200000000][i];
}

int
column_sum(int (*p)[3], int i)
{
        return (p[1] + 536870911)[i];
}

int *
column_wide(int (*p)[2][3], int i)
{
        return p[1][i];
}

int *
row_sum(int (*p)[3], int i)
{
        return p[1] + i;
}

int *
row_sum_first(int (*p)[3], int i)
{
        return i + p[1];
}

int *
row_sum_register(int (*p)[3], int i)
{
        register int *q;

        q = p[1] + i;
        return q;
}

void
row_sum_stored(int (*p)[3], int i, int **out)
{
        *out = p[1] + i;
}

int (*row_sum_wide(int (*p)[2][3], int i))[3]
{
        return p[1] + i;
}
