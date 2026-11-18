/* backend.md 6.3 and aarch64.md 6.3, m[i][2]: a constant over an
   lvalue's address is the displacement of that address, as over a
   name's.  The address of the row m[i] is the addressing form of a base
   and an index, and the constant 2 is its displacement; no instruction
   forms the row's address.  On a local array the slot is the base's
   part, and on a row by a constant index the two constants are one,
   p[1][2] on int (*p)[3] being 12 + 8 bytes.

   On x86-64 the listing must show row() reading p[i][2] by the one
   movsbl 2(%rax,%rcx,8), %eax after p's load and i's movslq, local()
   reading m[i][2] by the one movsbl -34(%rbp,%rax,8), %eax, the slot
   plus 2, after i's movslq, cell() reading p[1][2] by the one
   movl 20(%rax), %eax after p's load, address() forming &p[i][2] by
   the one leaq 2(%rax,%rcx,8), %rax, and corner() forming &p[1][2] by
   the one leaq 20(%rax), %rax.

   On aarch64 the listing must show cell() reading p[1][2] by the one
   ldr w0, [x0, #20] after p's load, and corner() forming &p[1][2] by
   the one add x0, x0, #20.  The machine has no form with an index and
   a displacement, so row() forms the row's address by add x0, x0, w1,
   sxtw #3 and reads ldrb w0, [x0, #2], local() adds the index to x29,
   add x0, x29, w0, sxtw #3, and reads ldrb w0, [x0, #22], the slot's
   20 and the 2, and address() adds the 2 with add x0, x0, #2. */
int
row(char (*p)[8], int i)
{
        return p[i][2];
}

int
local(int i)
{
        char m[4][8];

        return m[i][2];
}

int
cell(int (*p)[3])
{
        return p[1][2];
}

char *
address(char (*p)[8], int i)
{
        return &p[i][2];
}

int *
corner(int (*p)[3])
{
        return &p[1][2];
}
