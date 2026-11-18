/* backend.md 6.1 and 7.2, a return names the result register: the
   value of return e is computed so that its last instruction writes
   rax where it can, and the operands generated before that
   instruction stay out of rax where nothing within them pins a
   register.  copied() returns r - p[i] on a register variable r, whose
   copy the subl writes; extended() a remainder as a long; reloaded()
   x + (c ? 1 : 2), whose x is spilled before the branch;
   compared() a < b / c and negated() !(x % y), whose setcc follows a
   cmp of operands outside rax; stored() and added() the value of an
   assignment and of += to an element; stepped() and
   stepped_element() the old value of x++ and of p[i]++; divided()
   (a + b / c) / d, whose dividend is held outside rax; multiplied()
   r * 4 on a register variable r, which the imul reads.  As
   reloaded(), a left operand spilled before the branch of its right
   operand: remainder_reloaded() x % (c ? 3 : 5) as a long,
   compared_reloaded() a < (c ? 1 : 2), negated_reloaded() -(x + (c ?
   1 : 2)), difference_reloaded() p - (c ? q : r),
   indexed_reloaded() &p[c ? i : 0] and assigned_reloaded() y = x +
   (c ? 1 : 2) on a local y; and stored_widened() *p = x on an
   unsigned *p and a long x, returned as an unsigned long.  Beside
   them the rule's edge: called() r + f(x), whose call leaves its
   result in rax; deeper() r - p[i / 2], a division within the right
   operand; written() r + (r += 2), whose right operand writes r; and
   stored_held() p[i] = x - y, whose place holds p and i while x - y
   is computed.

   On x86-64 the listing must show no move into eax or rax before the
   jump to the epilogue but in called() and deeper(): in copied() p in
   rcx and i in rdx, movslq -20(%rbp), %rdx, then movl %ebx, %eax and
   subl (%rcx,%rdx,4), %eax; in extended() cltd, idivl -8(%rbp) and
   movslq %edx, %rax; in reloaded() x spilled from rax, the arms'
   movl $1, %ecx and movl $2, %ecx, then movq -16(%rbp), %rax and
   addl %ecx, %eax; in compared() a in ecx, the division and
   cmpl %eax, %ecx, then setl %al and movzbl %al, %eax; in negated()
   cmpl $0, %edx, sete %al and movzbl %al, %eax; in stored() p in rcx
   and i in rdx, movq -24(%rbp), %rax and movq %rax, (%rcx,%rdx,8); in
   added() x in rsi, movq (%rcx,%rdx,8), %rax, addq %rsi, %rax and the
   store from rax; in stepped() movl -4(%rbp), %ecx, movl %ecx, %eax,
   addl $1, %ecx and movl %ecx, -4(%rbp); in stepped_element() the
   element loaded into esi and copied into eax, movl %esi, %eax, then
   addl $1, %esi and the store from esi; in divided() a + b / c in ecx,
   movl %ecx, %eax, cltd and idivl -16(%rbp), with no move after it; in
   multiplied() imull $4, %ebx, %eax, and no copy of ebx; after the
   reload, in remainder_reloaded() idivl %ecx and movslq %edx, %rax, in
   compared_reloaded() cmpl %eax, %ecx and setl %al, in
   negated_reloaded() addl %ecx, %eax and negl %eax, in
   difference_reloaded() subq %rcx, %rax, cqto and idivq %rcx, in
   indexed_reloaded() leaq (%rcx,%rax,4), %rax and in
   assigned_reloaded() addl %ecx, %eax and movl %eax, -12(%rbp); in
   stored_widened() movl %eax, (%rcx) and movl %eax, %eax; in
   stored_held() subl -20(%rbp), %eax and the store from eax.  In
   called() the call, then movl %ebx, %ecx, addl %eax, %ecx and
   movl %ecx, %eax; in deeper() the quotient in eax, movl %ebx, %edx,
   subl (%rcx,%rax,4), %edx and movl %edx, %eax; in written() the copy
   preserve() makes, movl %ebx, %eax, then addl $2, %ebx and
   addl %ebx, %eax.

   On aarch64, where an operation names its destination apart from its
   sources, the row's last instruction writes x0 where no other value
   holds it, a source in it included: the listing must show sub w0,
   w19, w0 in copied(); msub w0, w2, w1, w0 and sxtw x0, w0 in
   extended(); ldr x1, [x29, #24] and add w0, w1, w0 in reloaded();
   cset w0, lt in compared(); cset w0, eq in negated(); sdiv w0, w0, w1
   in divided(); mul w0, w19, w0 after mov w0, #4 in multiplied(); add
   w0, w19, w0 in called(); sub w0, w19, w0 in deeper(); in written()
   the copy, mov w0, w19, and add w0, w0, w19; after the reload of the
   left operand into x1, in remainder_reloaded() sdiv w2, w1, w0, msub
   w0, w2, w0, w1 and sxtw x0, w0, in compared_reloaded() cmp w1, w0
   and cset w0, lt, in negated_reloaded() add w1, w1, w0 and neg w0,
   w1, in difference_reloaded() sub x1, x1, x0, mov x0, #4 and sdiv
   x0, x1, x0, in indexed_reloaded() add x0, x1, w0, sxtw #2 and in
   assigned_reloaded() add w0, w1, w0 and str w0, [x29, #24]; in
   stored_widened() str w1, [x0], then the extension mov w0, w1.  The
   return moves the value where the place of an assignment or of a
   step holds x0: in stored() mov x0, x2; in added() mov x0, x3; in
   stepped_element() mov w0, w2; in stored_held() sub w2, w2, w3, the
   store and mov w0, w2.  stepped() is ldr w0, [x29, #16], add w1, w0,
   #1 and str w1, [x29, #16]. */
int f(int);

int
copied(int *p, int i, int x)
{
        register int r = x;

        return r - p[i];
}

long
extended(int x, int y)
{
        return x % y;
}

int
reloaded(int x, int c)
{
        return x + (c ? 1 : 2);
}

int
compared(int a, int b, int c)
{
        return a < b / c;
}

int
negated(int x, int y)
{
        return !(x % y);
}

long
stored(long *p, int i, long x)
{
        return p[i] = x;
}

long
added(long *p, int i, long x)
{
        return p[i] += x;
}

int
stepped(int x)
{
        return x++;
}

int
stepped_element(int *p, int i)
{
        return p[i]++;
}

int
divided(int a, int b, int c, int d)
{
        return (a + b / c) / d;
}

int
multiplied(int x)
{
        register int r = x;

        return r * 4;
}

int
called(int x)
{
        register int r = x;

        return r + f(x);
}

int
deeper(int *p, int i, int x)
{
        register int r = x;

        return r - p[i / 2];
}

int
written(int x)
{
        register int r = x;

        return r + (r += 2);
}

long
remainder_reloaded(int x, int c)
{
        return x % (c ? 3 : 5);
}

int
compared_reloaded(int a, int c)
{
        return a < (c ? 1 : 2);
}

int
negated_reloaded(int x, int c)
{
        return -(x + (c ? 1 : 2));
}

long
difference_reloaded(int *p, int *q, int *r, int c)
{
        return p - (c ? q : r);
}

int *
indexed_reloaded(int *p, int i, int c)
{
        return &p[c ? i : 0];
}

int
assigned_reloaded(int x, int c)
{
        int y;

        return y = x + (c ? 1 : 2);
}

unsigned long
stored_widened(unsigned *p, long x)
{
        return *p = x;
}

int
stored_held(int *p, int i, int x, int y)
{
        return p[i] = x - y;
}
