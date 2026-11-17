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
   (a + b / c) / d, whose dividend is held outside rax.  Beside them
   the rule's edge: called() r + f(x), whose call leaves its result in
   rax; deeper() r - p[i / 2], a division within the right operand;
   and written() r + (r += 2), whose right operand writes r.

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
   movl %ecx, %eax, cltd and idivl -16(%rbp), with no move after it.  In
   called() the call, then movl %ebx, %ecx, addl %eax, %ecx and
   movl %ecx, %eax; in deeper() the quotient in eax, movl %ebx, %edx,
   subl (%rcx,%rax,4), %edx and movl %edx, %eax; in written() the copy
   preserve() makes, movl %ebx, %eax, then addl $2, %ebx and
   addl %ebx, %eax.

   On aarch64, where an operation names its destination apart from its
   sources, the listing must show sub w0, w19, w0 in copied(); msub w0,
   w2, w1, w0 and sxtw x0, w0 in extended(); cset w0, lt in compared();
   cset w0, eq in negated(); sdiv w0, w0, w1 in divided(); add w0, w19,
   w0 in called(); sub w0, w19, w0 in deeper(); and in written() the
   copy, mov w0, w19, and add w0, w0, w19.  The return moves the value
   where an operand generated first took x0: in reloaded() ldr x1,
   [x29, #24], add w1, w1, w0 and mov w0, w1; in stored() mov x0, x2;
   in added() mov x0, x3; in stepped_element() mov w0, w2.  stepped()
   is ldr w0, [x29, #16], add w1, w0, #1 and str w1, [x29, #16]. */
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
