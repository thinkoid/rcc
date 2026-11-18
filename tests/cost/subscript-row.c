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
   by, the row p[1][i] of 12 bytes on int (*p)[2][3], is on x86-64
   the index's product at scale 1 beside the pointer, the row's offset
   the displacement: p[1][i][0] on int (*p)[3][3] reads 36 bytes past
   the pointer and the product, p[1][i][2] 44, and on a local array
   the slot is the displacement off rbp, with no address formed;
   i[p[1]][0], the index written first, forms the row's address and
   the sum.  The row plus a variable index as a value, p[1] + i, is
   the same address and one leaq of it on x86-64, returned, i + p[1]
   with the index generated first, into a register variable, and
   stored; at a size no addressing form scales by, the 12 bytes of
   p[1] + i on int (*p)[2][3], it is the one leaq of the pointer, the
   product and the row's offset.  On aarch64, where one form holds a
   register offset or a displacement and never both, the product is
   added to the pointer and the row's offset is the access's
   displacement past the sum, a label's its offset in the page; with
   no constant at all the product is the access's register offset
   instead, p[i][0] in row_start(), but not where the address is held
   across a right operand, p[i][0] = f() in row_start_stored().  A row
   under a cast and past a constant of bytes, ((int (*)[3]) (c +
   4))[i][2] in row_under_cast(), or past a constant of rows, (p +
   1)[i][2] in row_past_rows(), takes that constant into the access's
   displacement past the sum as well; a label whose offset with the
   column is no multiple of the access's size keeps its page's add,
   ((int (*)[3]) ((char *) cells + 2))[i][1] in row_label_refused(); and
   constants that cancel the column leave the product the register
   offset, ((int (*)[1]) (c + 4))[i][-1] in row_cancelled().

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
   2147483644(%rax,%rcx,4), %eax, and column_wide() forming p[1][i]
   by imulq $12, %rcx, %rcx and the one leaq 24(%rax,%rcx,1), %rax
   after p's load and i's.  row_sum() forms p[1] + i by the one
   leaq 12(%rax,%rcx,4), %rax after p's load and i's, row_sum_first()
   by leaq 12(%rcx,%rax,4), %rax after i's load and p's,
   row_sum_register() by leaq 12(%rax,%rcx,4), %rbx into the variable,
   row_sum_stored() by leaq 12(%rcx,%rdx,4), %rcx before the store
   movq %rcx, (%rax), and row_sum_wide() as column_wide().  cell()
   reads p[1][i][0] by imulq $12, %rcx, %rcx and the one movl
   36(%rax,%rcx,1), %eax after p's load and i's, with no leaq and no
   addq, cell_column() the same with movl 44(%rax,%rcx,1), %eax,
   cell_register() with movl 36(%rbx,%rax,1), %eax, and cell_first()
   by leaq 36(%rcx), %rcx and addq %rcx, %rax before movl (%rax),
   %eax.  On the arrays of rows of 5 chars and of 3 ints, slot_cell()
   reads m[i][4] by imulq $5, %rax, %rax and the one movsbl
   -20(%rbp,%rax,1), %eax, with no leaq of the slot, label_cell()
   rows[i][2] by leaq rows(%rip), %rax, then the imulq and movsbl
   2(%rax,%rcx,1), %eax, pointer_cell() p[i][1] by movl
   4(%rax,%rcx,1), %eax after the imulq, long_cell() p[n][2] by imulq
   $12, -16(%rbp), %rcx, n the multiply's source, and movl
   8(%rax,%rcx,1), %eax, and slot_row() m[i] as a pointer by the one
   leaq -52(%rbp,%rax,1), %rax after the imulq.  first_cell(), i[p][1]
   with the index first, keeps the addq -8(%rbp), %rax of p in memory
   after the imulq, then movl 4(%rax), %eax; held_cell(), m[i][1] -=
   f(), whose address is held across the call, forms it in one
   register, leaq -24(%rbp), %rax, the imulq and addq %rcx, %rax,
   which the call spills once, then subb %al, 1(%rcx); and
   register_row(), q = m[i] into a register variable, is the imulq
   and the one leaq -60(%rbp,%rax,1), %rbx into it.  On char (*p)[8],
   byte_sum() forms p[1] + i by the one leaq 8(%rax,%rcx,1), %rax
   after p's load and i's, byte_sum_register() the same into the
   variable, leaq 8(%rax,%rcx,1), %rbx, and byte_sum_long(), whose
   long n is in memory, by leaq 8(%rax), %rax and addq -16(%rbp),
   %rax.  row_start() reads p[i][0] by the imulq and movl
   (%rax,%rcx,1), %eax, and row_start_stored() forms the address by
   addq %rcx, %rax, held across the call, then movl %eax, (%rcx).
   row_under_cast() reads by imulq $12, %rcx, %rcx and the one movl
   12(%rax,%rcx,1), %eax after c's load and i's, row_past_rows() by the
   same and movl 20(%rax,%rcx,1), %eax, row_label_refused() by leaq
   cells+2(%rip), %rax, the imulq and movl 4(%rax,%rcx,1), %eax, and
   row_cancelled() by the one movl (%rax,%rcx,4), %eax.

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
   row_sum_register(), and row_sum_wide() as column_wide().  cell(),
   cell_column(), cell_register() and cell_first() add the product to
   the pointer, mov x2, #12, mul and add x0, x0, x1, x19's sum written
   into x0 in cell_register() and the index first in cell_first(), and
   read with the row's 36 the load's offset, ldr w0, [x0, #36], and
   ldr w0, [x0, #44] in cell_column(), with no add of the 36.  On the
   rows of 5 chars and of 3 ints, slot_cell() adds the product to x29,
   add x0, x29, x0, and reads by ldrb w0, [x0, #24], the slot's 20 and
   the column's 4; label_cell() is adrp x0, rows+2, the add of the
   product and ldrb w0, [x0, :lo12:rows+2], with no add of the page's
   offset; held_cell() the same sum off x29, held across the call, and
   ldrb w1, [x2, #21] and strb w1, [x2, #21]; pointer_cell() and
   first_cell(), whose pointer has no constant, the add of the product
   and ldr w0, [x0, #4]; slot_row() the add of the slot and the add of
   the product, and register_row() the same, the last add writing
   x19.  row_start() reads p[i][0] by the register-offset form, ldr
   w0, [x0, x1] after the mul, with no add, and row_start_stored(),
   whose address is held across the call, adds the product and stores
   by str w0, [x1].  byte_sum(),
   byte_sum_long() and byte_sum_register() add the row's 8, then the
   index, add x0, x0, w1, sxtw, or add x0, x0, x1 for the long, the
   last add writing x19 in byte_sum_register().  row_under_cast() and
   row_past_rows() add the product to the pointer, mov x2, #12, mul and
   add x0, x0, x1, with no add of the 4 or the 12, and read by ldr w0,
   [x0, #12] and ldr w0, [x0, #20]; row_label_refused(), whose 2 and 4
   are no multiple of the int's 4 bytes in the page, keeps adrp x0,
   cells+2 and add x0, x0, :lo12:cells+2 before the product's add and
   ldr w0, [x0, #4]; and row_cancelled() is ldr w0, [x0, w1, sxtw #2]
   after c's load and i's, with no add. */
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

int
cell(int (*p)[3][3], int i)
{
        return p[1][i][0];
}

int
cell_column(int (*p)[3][3], int i)
{
        return p[1][i][2];
}

int
cell_register(int (*s)[3][3], int i)
{
        register int (*p)[3][3] = s;

        return p[1][i][0];
}

int
cell_first(int (*p)[3][3], int i)
{
        return i[p[1]][0];
}

char rows[4][5];
int f(void);

int
slot_cell(int i)
{
        char m[4][5];

        return m[i][4];
}

int
label_cell(int i)
{
        return rows[i][2];
}

int
pointer_cell(int (*p)[3], int i)
{
        return p[i][1];
}

int
long_cell(int (*p)[3], long n)
{
        return p[n][2];
}

int *
slot_row(int i)
{
        int m[4][3];

        return m[i];
}

int
first_cell(int (*p)[3], int i)
{
        return i[p][1];
}

void
held_cell(int i)
{
        char m[4][5];

        m[i][1] -= f();
}

int *
register_row(int i)
{
        int m[4][3];
        register int *q;

        q = m[i];
        return q;
}

char *
byte_sum(char (*p)[8], int i)
{
        return p[1] + i;
}

char *
byte_sum_long(char (*p)[8], long n)
{
        return p[1] + n;
}

char *
byte_sum_register(char (*p)[8], int i)
{
        register char *q;

        q = p[1] + i;
        return q;
}

int
row_start(int (*p)[3], int i)
{
        return p[i][0];
}

void
row_start_stored(int (*p)[3], int i)
{
        p[i][0] = f();
}

int
row_under_cast(char *c, int i)
{
        return ((int (*)[3]) (c + 4))[i][2];
}

int
row_past_rows(int (*p)[3], int i)
{
        return (p + 1)[i][2];
}

int cells[4][3];

int
row_label_refused(int i)
{
        return ((int (*)[3]) ((char *) cells + 2))[i][1];
}

int
row_cancelled(char *c, int i)
{
        return ((int (*)[1]) (c + 4))[i][-1];
}
