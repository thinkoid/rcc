/* backend.md 6.3 and aarch64.md 6.3, m[i][2]: a constant over an
   lvalue's address is the displacement of that address, as over a
   name's.  The address of the row m[i] is the addressing form of a base
   and an index, and the constant 2 is its displacement; no instruction
   forms the row's address.  On a local array the slot is the base's
   part, and on a row by a constant index the two constants are one,
   p[1][2] on int (*p)[3] being 12 + 8 bytes.  byte_view(),
   byte_view_store() and byte_view_near() read and write an int
   through a char pointer plus a byte offset under a cast,
   *(int *) (p + 4097), and read *(int *) (p + 1).  far_called() and
   far_stored() store through a constant index no displacement of the
   access holds, p[5000] = g() and p[5000] = v on an int *p.

   On x86-64 the listing must show row() reading p[i][2] by the one
   movsbl 2(%rax,%rcx,8), %eax after p's load and i's movslq, local()
   reading m[i][2] by the one movsbl -34(%rbp,%rax,8), %eax, the slot
   plus 2, after i's movslq, cell() reading p[1][2] by the one
   movl 20(%rax), %eax after p's load, address() forming &p[i][2] by
   the one leaq 2(%rax,%rcx,8), %rax, and corner() forming &p[1][2] by
   the one leaq 20(%rax), %rax.  byte_view() adds the byte offset,
   addq $4097, %rax, then reads movl (%rax), %eax, byte_view_store()
   the same before movl %ecx, (%rax), and byte_view_near() addq $1,
   %rax and movl (%rax), %eax.  far_called() stores movl %eax,
   20000(%rcx) after the call and p's reload, and far_stored() movl
   %ecx, 20000(%rax).

   On aarch64 the listing must show cell() reading p[1][2] by the one
   ldr w0, [x0, #20] after p's load, and corner() forming &p[1][2] by
   the one add x0, x0, #20.  The machine has no form with an index and
   a displacement, so row() forms the row's address by add x0, x0, w1,
   sxtw #3 and reads ldrb w0, [x0, #2], local() adds the index to x29,
   add x0, x29, w0, sxtw #3, and reads ldrb w0, [x0, #22], the slot's
   20 and the 2, and address() adds the 2 with add x0, x0, #2.  Under
   the cast the byte offset is the access's: byte_view() holds 4097,
   which no displacement reaches, and reads it as the register offset,
   mov x1, #4097 and ldr w0, [x0, x1], with no add; byte_view_store(),
   whose address is held across the right operand, keeps to one
   register, add x0, x0, x1, then str w1, [x0] after v's load; and
   byte_view_near() reads by the one ldr w0, [x0, #1].  The constant
   index of a store is written after its right operand, since it reads
   nothing: far_called() spills p alone across the call, then bl g;
   mov x1, #5000; ldr x2, [x29, #24]; str w0, [x2, x1, lsl #2], and
   far_stored() is mov x2, #5000 after v's load, then str w1, [x0, x2,
   lsl #2]. */
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

int
byte_view(char *p)
{
        return *(int *) (p + 4097);
}

void
byte_view_store(char *p, int v)
{
        *(int *) (p + 4097) = v;
}

int
byte_view_near(char *p)
{
        return *(int *) (p + 1);
}

int g(void);

void
far_called(int *p)
{
        p[5000] = g();
}

void
far_stored(int *p, int v)
{
        p[5000] = v;
}
