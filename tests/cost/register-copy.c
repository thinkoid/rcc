/* backend.md 6.1 and chapter 4, and aarch64.md chapter 4: a register
   variable's read as the left operand of an operator that commutes,
   whose right operand is an owned value in a register.  stored() g =
   r + f(x) to a global, local() s = r * f(x) to a local, argument()
   f(r & f(x)) to an argument's slot, destination() q = r | f(x) into
   another register variable q, and chained() s = (r ^ f(x)) + y, a
   further row's left operand: no register is named for the value.
   Beside them the rule's edges: both() r + q, whose right operand is
   another register variable's read; in_place() r += f(x), written in
   place; named() return r + y * z, whose return names a free rax;
   reserved() s = (r + f(y)) + y / z, whose division leaves rax for
   last; minus() r - f(x), whose operator does not commute; and
   operands() r + 5 and r + g, a constant and an object in memory as
   the right operand.  And the pointer's row: row() h = p + f(x) on a
   register int (*p)[3], whose rows of 12 bytes the index's product
   steps.

   On x86-64 the listing must show, after each call, the row writing
   the call's eax with r's register its source and no copy of ebx:
   addl %ebx, %eax and movl %eax, g(%rip) in stored(); imull %ebx,
   %eax and the store from eax in local(); andl %ebx, %eax and the
   argument's slot written from eax in argument(); orl %ebx, %eax and
   movl %eax, %r12d in destination(); xorl %ebx, %eax, then addl
   -20(%rbp), %eax in chained().  It must show the copy kept in the
   edges: movl %ebx, %eax; addl %r12d, %eax in both(); addl %eax, %ebx
   in in_place(), the variable written in place; y * z in ecx, then
   movl %ebx, %eax; addl %ecx, %eax in named(), the copy in the
   register the return names; movl %ebx, %ecx; addl %eax, %ecx before
   the dividend's load into eax in reserved(); movl %ebx, %ecx; subl
   %eax, %ecx in minus(); and movl %ebx, %eax; addl $5, %eax and movl
   %ebx, %eax; addl g(%rip), %eax in operands().  In row() it must
   show imulq $12, %rax, %rax after the call's result is widened, then
   addq %rbx, %rax and movq %rax, h(%rip), with no copy of rbx.

   On aarch64, where an operation names its destination apart from its
   sources, the listing must show no copy at all: add w0, w19, w0 in
   stored(), mul w0, w19, w0 in local(), and w0, w19, w0 in
   argument(), orr w20, w19, w0 in destination(), eor w0, w19, w0 in
   chained(), add w0, w19, w20 in both(), add w19, w19, w0 in
   in_place(), add w0, w19, w0 in named() and reserved(), sub w0, w19,
   w0 in minus(), and add w0, w19, #5 and add w0, w19, w0 after g's
   load in operands().  In row() it must show the product by a held
   12 and add x0, x19, x0 before the store to h. */
int f(int);
int g;
int (*h)[3];

void
stored(int a, int x)
{
        register int r = a;

        g = r + f(x);
}

int
local(int a, int x)
{
        register int r = a;
        int s;

        s = r * f(x);
        return s;
}

void
argument(int a, int x)
{
        register int r = a;

        f(r & f(x));
}

int
destination(int a, int x)
{
        register int r = a;
        register int q;

        q = r | f(x);
        return q ^ r;
}

int
chained(int a, int x, int y)
{
        register int r = a;
        int s;

        s = (r ^ f(x)) + y;
        return s;
}

int
both(int a, int b)
{
        register int r = a;
        register int q = b;
        int s;

        s = r + q;
        return s;
}

int
in_place(int a, int x)
{
        register int r = a;

        r += f(x);
        return r;
}

int
named(int a, int y, int z)
{
        register int r = a;

        return r + y * z;
}

int
reserved(int a, int y, int z)
{
        register int r = a;
        int s;

        s = (r + f(y)) + y / z;
        return s;
}

int
minus(int a, int x)
{
        register int r = a;
        int s;

        s = r - f(x);
        return s;
}

int
operands(int a)
{
        register int r = a;
        int s;
        int t;

        s = r + 5;
        t = r + g;
        return s + t;
}

void
row(int (*a)[3], int x)
{
        register int (*p)[3] = a;

        h = p + f(x);
}
