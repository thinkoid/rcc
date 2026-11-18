/* aarch64.md chapter 4, an operation into a register variable on its
   left operand there: the left operand's row writes the variable's
   register, and the operation reads it there, where the right operand
   reads nothing the left operand's row could overwrite.
   constant_left() writes r = 5 - s on two register variables, whose
   constant is the constant row into r's register, and
   constant_shift() r = 5 << s; constant_wide() r = 0x12345678 - s,
   whose constant is two instructions, both writing the register
   before the operation reads it; beside them constant_itself() r = 5
   - r, whose right operand is the variable, and keeps its constant
   elsewhere.  retyped_twice() writes v = 5 + (int) u into a register
   unsigned v from an unsigned in memory, a cast of the signedness
   alone under the assignment's own, which hands the variable on as
   the first does.  difference() writes w = q - a on a register int *q
   and an array into a register long w, whose sub writes the variable
   before the sdiv divides it in place, and difference_char() the same
   on chars, whose sub is the row's last instruction.  Each function's
   variable is r, v or w, in the second register variable's register,
   the first's being s or q, and in constant_itself() and
   retyped_twice() the first.

   On x86-64, which computes each in the variable's register by its
   own rules (backend.md chapter 4), the listing must show
   constant_left() movl $5, %r12d and subl %ebx, %r12d,
   constant_wide() movl $305419896, %r12d and the subl,
   constant_itself() movl $5, %eax, subl %ebx, %eax and movl %eax,
   %ebx, constant_shift() movl $5, %r12d and shll %cl, %r12d,
   retyped_twice() movl -12(%rbp), %ebx and addl $5, %ebx,
   difference() subq %rcx, %rax, cqto, idivq %rcx and movq %rax, %r12,
   and difference_char() subq %rax, %rcx and movq %rcx, %r12.

   On aarch64 the listing must show constant_left() mov w20, #5 and
   sub w20, w20, w19, constant_wide() mov w20, #22136, movk w20,
   #0x1234, lsl #16 and sub w20, w20, w19, constant_itself() mov w0,
   #5 and sub w19, w0, w19, constant_shift() mov w20, #5 and lsl w20,
   w20, w19, retyped_twice() ldr w19, [x29, #24] and add w19, w19, #5,
   difference() sub x20, x19, x0, mov x0, #4 and sdiv x20, x20, x0,
   and difference_char() the one sub x20, x19, x0, no expression
   register taken for any of them but constant_itself()'s w0, the
   right operand's x0 and the size's. */
int
constant_left(int a, int b)
{
        register int s = a;
        register int r;

        r = 5 - s;
        return r + b;
}

int
constant_wide(int a, int b)
{
        register int s = a;
        register int r;

        r = 0x12345678 - s;
        return r + b;
}

int
constant_itself(int a)
{
        register int r = a;

        r = 5 - r;
        return r;
}

int
constant_shift(int a, int b)
{
        register int s = a;
        register int r;

        r = 5 << s;
        return r + b;
}

unsigned
retyped_twice(unsigned u, unsigned b)
{
        register unsigned v;

        v = 5 + (int) u;
        return v + b;
}

long
difference(int *p, long b)
{
        int a[4];
        register int *q = p;
        register long w;

        a[0] = 0;
        w = q - a;
        return w + b;
}

long
difference_char(char *p, long b)
{
        char a[4];
        register char *q = p;
        register long w;

        a[0] = 0;
        w = q - a;
        return w + b;
}
