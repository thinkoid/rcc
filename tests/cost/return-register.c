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
   (c ? 1 : 2) on a local y; stored_widened() *p = x on an unsigned *p
   and a long x, returned as an unsigned long; stored_held() p[i] = x
   - y, whose place holds p and i while x - y is computed;
   incremented_element() ++p[i]; and compound_reloaded() y += (c ? 1 :
   2) on a local y.  A right operand that writes the register variable
   the left operand reads has the read's copy in rax: written() r + (r
   += 2), written_branched() r + (c ? r++ : 2), whose read is saved
   before the branch and reloaded, and written_nested() r + (y + (r =
   2)), whose copy is made within an operand of the right one.  An
   operator that commutes writes its right operand's register where the
   return names it: called() r + f(x) and written_called() r + (r =
   f(x)), whose call leaves its result in rax, and compound_called() x
   += f(y) on a local.  Beside them the rule's edge: called_minus() r -
   f(x), whose operator does not commute; deeper() r - p[i / 2], a
   division within the right operand; and called_place() p[f(i)] = x,
   whose place holds a call's result.  The pointer rows take the same
   copy: written_sum() and written_row() p + (i + (p = t, 0)) on a
   register char *p and on a register int (*p)[3], whose rows of 12
   bytes the index's product steps, written_difference() p - (q + (p =
   t, 0)) on a register char *p, and written_quotient() p - (p = t) on
   a register int *p, whose copy is the dividend; beside them
   written_sum_called() and written_quotient_called(), a call within
   the right operand.  A pointer plus a call's result writes the
   result's register too: pointer_called() p + f(x) on a char *p,
   row_called() the same on int (*p)[3], whose index's product is in
   rax, and register_row_called() on a register int (*p)[3], whose
   register the addq reads; beside them scaled_called() on an int *p,
   whose leaq names rax already, and called_pointer() f(x) + p, the
   index first, whose addq takes p in memory.

   On x86-64 the listing must show no move into eax or rax before the
   jump to the epilogue but in called_minus(), deeper() and
   called_place():
   in copied() p in rcx and i in rdx, movslq -20(%rbp), %rdx, then movl
   %ebx, %eax and subl (%rcx,%rdx,4), %eax; in extended() cltd, idivl
   -8(%rbp) and movslq %edx, %rax; in reloaded() x spilled from rax,
   the arms' movl $1, %ecx and movl $2, %ecx, then movq -16(%rbp), %rax
   and addl %ecx, %eax; in compared() a in ecx, the division and cmpl
   %eax, %ecx, then setl %al and movzbl %al, %eax; in negated() cmpl
   $0, %edx, sete %al and movzbl %al, %eax; in stored() p in rcx and i
   in rdx, movq -24(%rbp), %rax and movq %rax, (%rcx,%rdx,8); in
   added() x in rsi, movq (%rcx,%rdx,8), %rax, addq %rsi, %rax and the
   store from rax; in stepped() movl -4(%rbp), %ecx, movl %ecx, %eax,
   addl $1, %ecx and movl %ecx, -4(%rbp); in stepped_element() the
   element loaded into esi and copied into eax, movl %esi, %eax, then
   addl $1, %esi and the store from esi; in divided() a + b / c in eax,
   the quotient's register, addl %ecx, %eax, then cltd and idivl
   -16(%rbp), with no move; in
   multiplied() imull $4, %ebx, %eax, and no copy of ebx; after the
   reload, in remainder_reloaded() idivl %ecx and movslq %edx, %rax, in
   compared_reloaded() cmpl %eax, %ecx and setl %al, in
   negated_reloaded() addl %ecx, %eax and negl %eax, in
   difference_reloaded() subq %rcx, %rax, cqto and idivq %rcx, in
   indexed_reloaded() leaq (%rcx,%rax,4), %rax and in
   assigned_reloaded() addl %ecx, %eax and movl %eax, -12(%rbp); in
   stored_widened() movl %eax, (%rcx) and movl %eax, %eax; in
   stored_held() subl -20(%rbp), %eax and the store from eax; in
   incremented_element() movl (%rcx,%rdx,4), %eax, addl $1, %eax and
   the store from eax; in compound_reloaded() movl -12(%rbp), %eax,
   addl %ecx, %eax and movl %eax, -12(%rbp).  In called_place() the
   store from ecx and movl %ecx, %eax.  In called() the call, then addl
   %ebx, %eax, r read in its register and no copy of it; in
   called_minus() the call, then movl %ebx, %ecx, subl %eax, %ecx and
   movl %ecx, %eax; in compound_called() the call, x loaded into ecx,
   addl %ecx, %eax and the store from eax; in deeper() the
   quotient in eax, movl %ebx, %edx, subl (%rcx,%rax,4), %edx and movl
   %edx, %eax; in written() the copy preserve() makes, movl %ebx, %eax,
   then addl $2, %ebx and addl %ebx, %eax; in written_branched() movq
   %rbx, -24(%rbp) before the branch, the arms' movl %ebx, %ecx and movl
   $2, %ecx, then movq -24(%rbp), %rax and addl %ecx, %eax; in
   written_nested() y loaded into ecx, the copy movl %ebx, %eax, then
   addl %ebx, %ecx and addl %ecx, %eax; in written_called() the copy
   spilled across the call, movq -24(%rbp), %rcx, and addl %ecx, %eax.
   In written_sum() i in ecx, the copy movq %rbx, %rax, then addq
   %rcx, %rax; in written_row() the same after imulq $12, %rcx, %rcx;
   in written_difference() q in rcx, the copy movq %rbx, %rax, then
   subq %rcx, %rax; in written_quotient() the copy movq %rbx, %rax,
   subq %rbx, %rax, movq $4, %rcx, cqto and idivq %rcx; in
   written_sum_called() and written_quotient_called() the copy spilled
   across the call and reloaded, movq -48(%rbp), %rax and movq
   -40(%rbp), %rax, before the addq and the subq.  In pointer_called()
   p reloaded into rcx after the call, movq -24(%rbp), %rcx, and addq
   %rcx, %rax; in row_called() the same after imulq $12, %rax, %rax;
   in register_row_called() imulq $12, %rax, %rax and addq %rbx, %rax,
   with no copy of rbx; in scaled_called() leaq (%rcx,%rax,4), %rax;
   and in called_pointer() addq -8(%rbp), %rax.

   On aarch64, where an operation names its destination apart from its
   sources, the row's last instruction writes x0 where no other value
   holds it, a source in it included, and the place of an assignment
   or of a step is generated outside x0: the listing must show sub w0,
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
   assigned_reloaded() add w0, w1, w0 and str w0, [x29, #24]; with p
   in x1 and i in w2, in stored() x loaded into x0 and str x0, [x1,
   w2, sxtw #3], in added() x in x0, ldr x3, [x1, w2, sxtw #3], add x0,
   x3, x0 and the store from x0, in stepped_element() ldr w0, [x1, w2,
   sxtw #2], add w3, w0, #1 and the store from w3, in stored_held() sub
   w0, w0, w3 and the store from w0, and in incremented_element() ldr
   w0, [x1, w2, sxtw #2], add w0, w0, #1 and the store from w0; in
   stored_widened() p in x1, str w0, [x1] and the extension mov w0, w0;
   in compound_reloaded() ldr w1, [x29, #24], add w0, w1, w0 and str
   w0, [x29, #24].  stepped() is ldr w0, [x29, #16], add w1, w0, #1 and
   str w1, [x29, #16].  In called_place() the index, the call's result,
   holds x0 at the store, str w1, [x2, w0, sxtw #2], and the return
   moves the value, mov w0, w1.  In written_branched() the reload ldr
   x1, [x29, #32] and add w0, w1, w0, in written_nested() the copy mov
   w1, w19 and add w0, w1, w0, in written_called() the reload after
   the call and add w0, w1, w0, in called_minus() sub w0, w19, w0, and
   in compound_called() ldr w1, [x29, #16], add w0, w1, w0 and str w0,
   [x29, #16].  In written_sum() and written_row() the copy mov x1, x19
   and add x0, x1, w0, sxtw and add x0, x1, x0, in written_difference()
   the copy mov x1, x19 and sub x0, x1, x0, in written_quotient() mov
   x0, x19, sub x0, x0, x19 and sdiv x0, x0, x1, and in
   written_sum_called() and written_quotient_called() the reload after
   the call, ldr x0, [x29, #56] and ldr x0, [x29, #48], before the add
   and the sub.  In pointer_called(), scaled_called() and
   called_pointer() the pointer reloaded into x1 after the call and
   add x0, x1, w0, sxtw, with #2 in scaled_called(); in row_called()
   mul x0, x0, x2 and add x0, x1, x0; and in register_row_called() add
   x0, x19, x0. */
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

int
incremented_element(int *p, int i)
{
        return ++p[i];
}

int
compound_reloaded(int x, int c)
{
        int y = x;

        return y += (c ? 1 : 2);
}

int
called_place(int *p, int i, int x)
{
        return p[f(i)] = x;
}

int
written_branched(int x, int c)
{
        register int r = x;

        return r + (c ? r++ : 2);
}

int
written_nested(int x, int y)
{
        register int r = x;

        return r + (y + (r = 2));
}

int
written_called(int x)
{
        register int r = x;

        return r + (r = f(x));
}

char *
written_sum(char *s, char *t, int i)
{
        register char *p = s;

        return p + (i + (p = t, 0));
}

int (*written_row(int (*s)[3], int (*t)[3], int i))[3]
{
        register int (*p)[3] = s;

        return p + (i + (p = t, 0));
}

long
written_difference(char *s, char *t, char *q)
{
        register char *p = s;

        return p - (q + (p = t, 0));
}

long
written_quotient(int *s, int *t)
{
        register int *p = s;

        return p - (p = t);
}

char *
written_sum_called(char *s, char *t, int i)
{
        register char *p = s;

        return p + (i + (p = t, f(i)));
}

long
written_quotient_called(int *s, int *t, int i)
{
        register int *p = s;

        return p - (p = t, t + f(i));
}

int
called_minus(int x)
{
        register int r = x;

        return r - f(x);
}

int
compound_called(int x, int y)
{
        return x += f(y);
}

char *
pointer_called(char *p, int x)
{
        return p + f(x);
}

int (*row_called(int (*p)[3], int x))[3]
{
        return p + f(x);
}

int *
scaled_called(int *p, int x)
{
        return p + f(x);
}

char *
called_pointer(char *p, int x)
{
        return f(x) + p;
}

int (*register_row_called(int (*s)[3], int x))[3]
{
        register int (*p)[3] = s;

        return p + f(x);
}
