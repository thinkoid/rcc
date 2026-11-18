/* backend.md 6.3 and aarch64.md 6.3, m[i][2]: a constant over an
   lvalue's address is the displacement of that address, as over a
   name's.  The address of the row m[i] is the addressing form of a base
   and an index, and the constant 2 is its displacement; no instruction
   forms the row's address.  On a local array the slot is the base's
   part, and on a row by a constant index the two constants are one,
   p[1][2] on int (*p)[3] being 12 + 8 bytes.  byte_view(),
   byte_view_store() and byte_view_near() read and write an int
   through a char pointer plus a byte offset under a cast,
   *(int *) (p + 4097), and read *(int *) (p + 1).  Beside them,
   under a cast: byte_index() a variable index, *(int *) (p + i) on a
   char *p, byte_scaled() *(long *) (s + i) on a short *s, whose
   stride is no access's size, byte_after_index() a constant after a
   variable index, *(short *) ((char *) p + i * 4 + 2),
   byte_register() a register pointer, byte_slot() and byte_label() a
   local array's and a global's address plus a constant; and the
   edges, byte_held() *(int *) (p + i) -= step(), whose address is
   held across the call, and byte_value() (int *) (p + 24), the
   address as a value.  far_called() and far_stored() store through a
   constant index no displacement of the access holds, p[5000] = g()
   and p[5000] = v on an int *p.  Over a sum under a cast, a
   subscript or a second constant joins the sum's own operand:
   view_subscript() ((int *) (p + 1))[3] and view_indexed() ((int *)
   (p + 1))[i] on a char *p, byte_both_sides() *(short *) ((char *) p
   + 4 + i * 4 + 2), constants on both sides of a variable index,
   two_casts() a chain of three casts; and the edges, two_indexes()
   *(short *) ((char *) p + i * 4 + j), two indexes, and view_beyond()
   ((int *) (p + 2000000000))[400000000], displacements no 32-bit
   field holds together.

   On x86-64 the listing must show row() reading p[i][2] by the one
   movsbl 2(%rax,%rcx,8), %eax after p's load and i's movslq, local()
   reading m[i][2] by the one movsbl -34(%rbp,%rax,8), %eax, the slot
   plus 2, after i's movslq, cell() reading p[1][2] by the one
   movl 20(%rax), %eax after p's load, address() forming &p[i][2] by
   the one leaq 2(%rax,%rcx,8), %rax, and corner() forming &p[1][2] by
   the one leaq 20(%rax), %rax.  Under the cast the sum's bytes are
   the access's operand: byte_view() reads by the one movl 4097(%rax),
   %eax after p's load, byte_view_store() stores by movl %ecx,
   4097(%rax), byte_view_near() reads movl 1(%rax), %eax,
   byte_index() movl (%rax,%rcx,1), %eax, byte_scaled() movq
   (%rax,%rcx,2), %rax, byte_after_index() movswl 2(%rax,%rcx,1),
   %eax after the product's movslq, byte_register() the one movl
   8(%rbx), %eax, byte_slot() movl -16(%rbp), %eax and byte_label()
   the one movl bytes+4(%rip), %eax; byte_held() keeps addq %rcx,
   %rax, spilled across the call, and subl %eax, (%rcx) after it, and
   byte_value() is addq $24, %rax.  far_called() stores movl %eax,
   20000(%rcx) after the call and p's reload, and far_stored() movl
   %ecx, 20000(%rax).  view_subscript() reads the one movl 13(%rax),
   %eax, view_indexed() movl 1(%rax,%rcx,4), %eax, byte_both_sides()
   movswl 6(%rax,%rcx,1), %eax and two_casts() the one movl 11(%rax),
   %eax, each after p's load and no add; two_indexes() adds the first
   index, addq %rcx, %rax, before movswl (%rax,%rcx,1), %eax, and
   view_beyond() forms the first part, leaq 2000000000(%rax), %rax,
   before movl 1600000000(%rax), %eax.

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
   byte_view_near() reads by the one ldr w0, [x0, #1].  byte_index()
   reads ldr w0, [x0, w1, sxtw]; byte_scaled(), whose stride is no
   access's size, adds the index, add x0, x0, w1, sxtw #1, before ldr
   x0, [x0]; byte_after_index() adds the product, add x0, x0, w1,
   sxtw, before ldrsh w0, [x0, #2]; byte_register() reads ldr w0,
   [x19, #8]; byte_slot() and byte_label() form the name's address,
   add x0, x29, #24 and the page's adrp and add, before ldr w0, [x0];
   byte_held() adds the index before the call, and byte_value() is
   add x0, x0, #24.  The constant index of a store is written after
   its right operand, since it reads nothing: far_called() spills p
   alone across the call, then bl g; mov x1, #5000; ldr x2, [x29,
   #24]; str w0, [x2, x1, lsl #2], and far_stored() is mov x2, #5000
   after v's load, then str w1, [x0, x2, lsl #2].  view_subscript()
   adds the 1 before ldr w0, [x0, #12], view_indexed() before ldr w0,
   [x0, w1, sxtw #2], byte_both_sides() adds the 4 and the product
   before ldrsh w0, [x0, #2], two_casts() adds 1, 4 and 2 before ldr
   w0, [x0, #4], two_indexes() adds the product before ldrsh w0, [x0,
   w1, sxtw], and view_beyond() holds both parts, the second as the
   register offset, ldr w0, [x0, x1, lsl #2]. */
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

char bytes[16];
int step(void);

int
byte_index(char *p, int i)
{
        return *(int *) (p + i);
}

long
byte_scaled(short *s, int i)
{
        return *(long *) (s + i);
}

int
byte_after_index(int *p, int i)
{
        return *(short *) ((char *) p + i * 4 + 2);
}

int
byte_register(char *s)
{
        register char *r = s;

        return *(int *) (r + 8);
}

int
byte_slot(int v)
{
        char c[16];

        c[0] = v;
        return *(int *) (c + 4);
}

int
byte_label(void)
{
        return *(int *) (bytes + 4);
}

void
byte_held(char *p, int i)
{
        *(int *) (p + i) -= step();
}

int *
byte_value(char *p)
{
        return (int *) (p + 24);
}

int
view_subscript(char *p)
{
        return ((int *) (p + 1))[3];
}

int
view_indexed(char *p, int i)
{
        return ((int *) (p + 1))[i];
}

int
byte_both_sides(int *p, int i)
{
        return *(short *) ((char *) p + 4 + i * 4 + 2);
}

int
two_casts(char *p)
{
        return ((int *) ((char *) ((short *) (p + 1) + 2) + 2))[1];
}

int
two_indexes(int *p, int i, int j)
{
        return *(short *) ((char *) p + i * 4 + j);
}

int
view_beyond(char *p)
{
        return ((int *) (p + 2000000000))[400000000];
}
