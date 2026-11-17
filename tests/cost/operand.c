/* backend.md 6.1, 6.3, 6.5 and 7.1, and aarch64.md 6.3: an object in
   memory as the instruction's own operand, after the Seventh Edition
   compiler's table, whose row %n,aw writes the left operand into the
   register and the instruction with the right operand's address.  add,
   sub, and, or, xor, mul, divide, modulo and less chain their operator
   with a right operand of each form, a local, a global the file
   defines, one it only declares, *p and a local array's a[i]; test does
   the same for & as a condition, and divide_busy divides by *p with a
   held and p, the divisor's address, loaded outside rax and rdx
   (backend.md 6.5).  less_constant, nonzero, nonzero_char and bit are
   the conditions on an object in memory and a constant.
   constant_left and constant_left_unsigned write each comparison with
   the constant 5 on the left of an int and of an unsigned in memory,
   constant_left_register of a register variable, constant_left_value
   for its value, and null_left is 0 == p: a constant on the left is
   taken as the constant on the right, the comparison mirrored
   (backend.md 6.1).  times_four is a multiply by a constant of an
   object in memory.  pointer_operand writes p - q and p + n at a
   stride of 1 and of 4, signedness u + i, u < i, u * i and u / i on an
   int under the cast to unsigned, and a widening cast beside them,
   comparison_value a comparison's value against a constant and against
   an object, signed and unsigned, and by_constant x * 4, x * -3, 4 * x
   and a long by a constant.  Beside them, char_operand, widened,
   compound, compound_value, shift_count and before_call keep the load:
   a char promoted, an int widened to long, a compound assignment's
   right operand for its effect and for its value, a shift's count and
   a right operand that is a call, which may write the global loaded
   before it.

   On x86-64 the listing must show each right operand as the
   instruction's operand, after the left operand's load and with no load
   of its own: in add, addl -24(%rbp), %eax for y, addl defined(%rip),
   %eax, the GOT's load movq declared@GOTPCREL(%rip), %rcx and
   addl (%rcx), %eax, p's load and addl (%rcx), %eax, the index's
   movslq -20(%rbp), %rcx and addl -40(%rbp,%rcx,4), %eax; subl, andl,
   orl, xorl and imull the same; cltd and idivl -24(%rbp) and the rest
   in divide, and in modulo the same, each remainder left in edx and
   moved into eax as the next dividend, movl %edx, %eax, after the
   divisor's address and before cltd, and the return's
   movl %edx, %eax after the last idivl; in less cmpl -24(%rbp), %eax,
   then cmpl defined(%rip), %ecx
   and the rest, each before its setl; in test testl -24(%rbp), %eax and
   the rest, each then je.  divide_busy must show a loaded into ecx, b
   into eax and p into rsi, then cltd, idivl (%rsi) and addl %eax,
   %ecx, and no move but the return's.  It must show cmpl $5,
   -4(%rbp) then jge in less_constant, cmpl $0, -4(%rbp) in nonzero,
   cmpb $0, -1(%rbp) in nonzero_char and testl $1, -4(%rbp) in bit,
   each then its jump and no load.  It must show the loads kept:
   movsbl -5(%rbp), %ecx before the addl in char_operand,
   movslq -12(%rbp), %rcx before the addq in widened, y loaded and
   added to the object, addl %eax, -4(%rbp), in compound, y loaded into
   ecx before x in compound_value, the count's movl -8(%rbp), %ecx
   before shll %cl in shift_count, and in before_call the load of
   defined before the call, its spill and its reload, then
   cmpl %eax, %ecx.  It must show no load of a constant on the left: in
   constant_left cmpl $5, -4(%rbp) six times, then jle, jl, jge, jg, jne
   and je, and the unsigned jumps jbe, jb, jae, ja, jne and je in
   constant_left_unsigned; cmpl $5, %ebx then jle in
   constant_left_register; cmpl $5, -4(%rbp) and setg %al, and no load,
   in constant_left_value; and cmpq $0, -8(%rbp) then jne in null_left.
   It must show the four rows beside the operand in memory with no load
   of the operand: imull $4, -4(%rbp), %eax in times_four; in
   pointer_operand subq -16(%rbp), %rcx for p - q after p's load,
   subq -32(%rbp), %rax for r - s before the division by 4,
   addq -40(%rbp), %rsi for p + n at a stride of 1, then
   subq -16(%rbp), %rsi, and for r + n at a stride of 4 n's load,
   movq -40(%rbp), %rdx, and leaq (%rax,%rdx,4), %rax, then
   subq -32(%rbp), %rax; in signedness addl -8(%rbp), %ecx,
   cmpl -8(%rbp), %esi before setb, imull -8(%rbp), %esi and
   xorl %edx, %edx; divl -8(%rbp), and for ul + j the widening load
   kept, movslq -20(%rbp), %rdx; addq %rdx, %rax; in comparison_value
   cmpl $5, -4(%rbp) before setl and cmpl $5, -8(%rbp) before setae,
   then cmpl -12(%rbp), %ecx before setl and cmpl -16(%rbp), %ecx
   before seta; in by_constant imull $4, -4(%rbp), %eax,
   imull $-3, -4(%rbp), %ecx and imulq $1000, -16(%rbp), %rcx, and
   4 * x as written, movl $4, %ecx; imull -4(%rbp), %ecx.

   On aarch64, a load-store machine with no instruction that takes a
   memory operand, the listing must show each operand loaded into a
   register, ldr w1, [x29, #36] for y, and each operation on registers,
   add w0, w0, w1 and the rest, sdiv w0, w0, w1, sdiv and msub in
   modulo, cmp w0, w1 before cset, and w0, w0, w1 before cbz in test;
   ldr then cmp w0, #5 in less_constant, cbz w0 in nonzero and
   nonzero_char, and w0, w0, #0x1 then cbz in bit.  It must show no
   mov of a constant on the left: ldr then cmp w0, #5 for each
   comparison, b.le, b.lt, b.ge, b.gt, b.ne and b.eq in constant_left,
   b.ls, b.lo, b.hs, b.hi, b.ne and b.eq in constant_left_unsigned;
   cmp w19, #5 then b.le in constant_left_register; cmp w0, #5 and
   cset w0, gt in constant_left_value; and ldr x0 then cmp x0, #0 and
   b.ne in null_left.  It must show the four rows' operands loaded as
   any operand is: sub x0, x0, x1 after both loads in pointer_operand,
   and add x1, x1, x2, lsl #2 for r + n; add, cmp, mul and udiv on
   registers after the loads in signedness, and ldrsw x2, [x29, #32]
   for the widened j; ldr then cmp w0, #5 before cset in
   comparison_value; and mov w1, #4 and mul w0, w0, w1 in by_constant,
   mov x2, #1000 for the long. */
int defined;
extern int declared;

int f(void);

int
add(int x, int *p, int i)
{
        int y = x;
        int a[4];

        return x + y + defined + declared + *p + a[i];
}

int
sub(int x, int *p, int i)
{
        int y = x;
        int a[4];

        return x - y - defined - declared - *p - a[i];
}

int
and(int x, int *p, int i)
{
        int y = x;
        int a[4];

        return x & y & defined & declared & *p & a[i];
}

int
or(int x, int *p, int i)
{
        int y = x;
        int a[4];

        return x | y | defined | declared | *p | a[i];
}

int
xor(int x, int *p, int i)
{
        int y = x;
        int a[4];

        return x ^ y ^ defined ^ declared ^ *p ^ a[i];
}

int
mul(int x, int *p, int i)
{
        int y = x;
        int a[4];

        return x * y * defined * declared * *p * a[i];
}

int
divide(int x, int *p, int i)
{
        int y = x;
        int a[4];

        return x / y / defined / declared / *p / a[i];
}

int
modulo(int x, int *p, int i)
{
        int y = x;
        int a[4];

        return x % y % defined % declared % *p % a[i];
}

int
less(int x, int *p, int i)
{
        int y = x;
        int a[4];

        return (x < y) + (x < defined) + (x < declared) + (x < *p) + (x < a[i]);
}

int
test(int x, int *p, int i)
{
        int y = x;
        int a[4];

        if ((x & y) && (x & defined) && (x & declared) && (x & *p) &&
            (x & a[i]))
                return 1;
        return 0;
}

int
divide_busy(int a, int b, int *p)
{
        return a + b / *p;
}

int
less_constant(int x)
{
        if (x < 5)
                return 1;
        return 0;
}

int
nonzero(int x)
{
        if (x)
                return 1;
        return 0;
}

int
nonzero_char(char c)
{
        if (c)
                return 1;
        return 0;
}

int
bit(int x)
{
        if (x & 1)
                return 1;
        return 0;
}

int
char_operand(int x, char c)
{
        return x + c;
}

long
widened(long l, int i)
{
        return l + i;
}

int
compound(int x, int y)
{
        x += y;
        return x;
}

int
compound_value(int x, int y)
{
        return x += y;
}

int
shift_count(int x, int n)
{
        return x << n;
}

int
times_four(int x)
{
        return x * 4;
}

int
before_call(void)
{
        if (defined < f())
                return 1;
        return 0;
}

int
constant_left(int x)
{
        int r = 0;

        if (5 < x)
                r += 1;
        if (5 <= x)
                r += 2;
        if (5 > x)
                r += 4;
        if (5 >= x)
                r += 8;
        if (5 == x)
                r += 16;
        if (5 != x)
                r += 32;
        return r;
}

int
constant_left_unsigned(unsigned x)
{
        int r = 0;

        if (5 < x)
                r += 1;
        if (5 <= x)
                r += 2;
        if (5 > x)
                r += 4;
        if (5 >= x)
                r += 8;
        if (5 == x)
                r += 16;
        if (5 != x)
                r += 32;
        return r;
}

int
constant_left_register(int a)
{
        register int x = a;

        if (5 < x)
                return 1;
        return 0;
}

int
constant_left_value(int x)
{
        return 5 < x;
}

int
null_left(int *p)
{
        if (0 == p)
                return 1;
        return 0;
}

long
pointer_operand(char *p, char *q, int *r, int *s, long n)
{
        return (p - q) + (r - s) + (p + n - q) + (r + n - s);
}

unsigned long
signedness(unsigned u, int i, unsigned long ul, int j)
{
        return (u + i) + (u < i) + u * i + u / i + (ul + j);
}

int
comparison_value(int x, unsigned u, int y, unsigned v)
{
        return (x < 5) + (u >= 5) + (x < y) + (u > v);
}

long
by_constant(int x, long l)
{
        return x * 4 + x * -3 + 4 * x + l * 1000;
}
