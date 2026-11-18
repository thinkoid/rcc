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
   field holds together.  A name's address plus a constant under a
   cast, the shape of a structure's member: member_slot() and
   member_odd() *(int *) ((char *) &x + 4) and + 1 on a local long
   x[2], member_far() *(int *) (c + 261) on a local char array, and
   member_label() and member_label_odd() *(short *) ((char *) words +
   6) and *(int *) ((char *) words + 6) on a global int words[8].
   view_address(), view_address_zero(), view_address_index() and
   view_address_register() are such addresses as values, &((int *) (p
   + 1))[3], &((int *) (p + 4))[-1], (short *) ((char *) p + 4 + i * 4
   + 2), and the first into a register int *q; view_address_held()
   &((long *) (p + 1))[4097], whose 32777 bytes no add encodes.

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
   before movl 1600000000(%rax), %eax.  The member shapes read the
   slot's or the label's own operand: member_slot() movl -20(%rbp),
   %eax, member_odd() movl -23(%rbp), %eax, member_far() movl
   -343(%rbp), %eax, member_label() movswl words+6(%rip), %eax and
   member_label_odd() movl words+6(%rip), %eax.  view_address() is the
   one leaq 13(%rax), %rax, view_address_zero() p's load alone,
   view_address_index() leaq 4(%rax,%rcx,1), %rax and addq $2, %rax,
   and view_address_register() addq $1, %rbx and leaq 12(%rbx), %rbx;
   view_address_held() the one leaq 32777(%rax), %rax.

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
   [x19, #8]; byte_slot() reads the slot's own ldr w0, [x29, #24], and
   byte_label() forms the name's address, the page's adrp and add,
   before ldr w0, [x0], its char array aligned to 1;
   byte_held() adds the index before the call, and byte_value() is
   add x0, x0, #24.  The constant index of a store is written after
   its right operand, since it reads nothing: far_called() spills p
   alone across the call, then bl g; mov x1, #5000; ldr x2, [x29,
   #24]; str w0, [x2, x1, lsl #2], and far_stored() is mov x2, #5000
   after v's load, then str w1, [x0, x2, lsl #2].  The constants under
   the cast and the subscript's are one displacement where it reaches:
   view_subscript() reads by the one ldr w0, [x0, #13] after p's load,
   and two_casts() by the one ldr w0, [x0, #11], with no add;
   byte_both_sides() adds the product alone, add x0, x0, w1, sxtw, and
   reads ldrsh w0, [x0, #6], the 4 past the sum with the 2.  One form
   holds a displacement or an index, never both, so view_indexed()
   adds the 1 before ldr w0, [x0, w1, sxtw #2] and two_indexes() the
   product before ldrsh w0, [x0, w1, sxtw]; and view_beyond(), whose
   sum no displacement reaches, holds both parts, the second as the
   register offset, ldr w0, [x0, x1, lsl #2].  A name's address plus
   a constant under a cast is the slot's or the label's own operand
   where the access encodes it: member_slot() reads ldr w0, [x29,
   #28], member_odd() the unscaled ldr w0, [x29, #25], and
   member_label() adrp x16, words+6; ldrsh w0, [x16, :lo12:words+6].
   member_far()'s offset, no multiple of 4 and beyond the unscaled 9
   bits, keeps add x0, x29, #281 before ldr w0, [x0], and
   member_label_odd(), whose label plus offset is no multiple of 4,
   keeps the page's adrp and add before ldr w0, [x0], since the scaled
   :lo12: form needs that multiple at link time.  As values the
   constants under the casts are one add: view_address() add x0, x0,
   #13 after p's load, view_address_zero() p's load alone,
   view_address_index() the product's add x0, x0, w1, sxtw, then add
   x0, x0, #6, and view_address_register() ldr x19, [x29, #24] and add
   x19, x19, #13; view_address_held() holds the sum, mov x1, #32777;
   add x0, x0, x1, where the parts were add x0, x0, #1 and the 32776
   held and added. */
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

int words[8];

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

int
member_slot(long v)
{
        long x[2];

        x[0] = v;
        return *(int *) ((char *) &x + 4);
}

int
member_odd(long v)
{
        long x[2];

        x[0] = v;
        return *(int *) ((char *) &x + 1);
}

int
member_far(int v)
{
        char c[600];

        c[261] = v;
        return *(int *) (c + 261);
}

int
member_label(void)
{
        return *(short *) ((char *) words + 6);
}

int
member_label_odd(void)
{
        return *(int *) ((char *) words + 6);
}

int *
view_address(char *p)
{
        return &((int *) (p + 1))[3];
}

int *
view_address_zero(char *p)
{
        return &((int *) (p + 4))[-1];
}

short *
view_address_index(int *p, int i)
{
        return (short *) ((char *) p + 4 + i * 4 + 2);
}

int *
view_address_register(char *p)
{
        register int *q;

        q = &((int *) (p + 1))[3];
        return q;
}

long *
view_address_held(char *p)
{
        return &((long *) (p + 1))[4097];
}
