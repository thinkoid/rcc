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
   and a long by a constant.  commuted writes a constant on the left of
   +, *, &, | and ^ of an int in memory and of a char promoted,
   commuted_right the same with the constant on the right,
   commuted_register of a register variable, commuted_call 5 + f(),
   commuted_into r = 4 * a and r = 5 + r into a register variable,
   commuted_char_register r = 5 + c on a register char c into one, and
   commuted_bit 1 & x as a condition: a constant on the left of an
   operator that commutes is taken as the constant on the right
   (backend.md 6.1).  commuted_wide, a constant no 32-bit immediate
   holds, is the boundary on x86-64, and not_commuted's 5 - x, 40 / x
   and 1 << x keep the order of the source.  Beside them, char_operand,
   widened, compound, compound_value, shift_count and before_call keep
   the load: a char promoted, an int widened to long, a compound
   assignment's right operand for its effect and for its value, a
   shift's count and a right operand that is a call, which may write the
   global loaded before it.

   index_sub, index_row, index_first and index_compound write the index
   multiply of pointer arithmetic, p - n at a size of 4, p + n and n + p
   at 12 and p -= n, n a long in memory, and index_int and index_scaled
   beside them, an int index and p + n at a scale; widen_signedness and
   widen_unsigned a widening over a cast that changes the signedness
   alone, (long) (int) u and (unsigned long) (unsigned) i, widen_char
   and widen_byte one over a cast that keeps every bit, (long) (char) c
   and (long) (signed char) uc, and widen_narrowed beside them over a
   narrowing cast; compare_register an
   object compared with a register variable, as a condition and as a
   value, and beside it an expression on the right; constant_index and
   constant_index_right 3 + p and p + 3, constant_index_into the same
   into a register pointer, from another and from itself, and
   constant_index_wide a constant whose product no 32-bit immediate
   holds (backend.md 6.1, 6.3).

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
   4 * x as x * 4 is, imull $4, -4(%rbp), %ecx.  It must show the same
   instructions in commuted and commuted_right: movl -4(%rbp), %eax;
   addl $5, %eax, imull $4, -4(%rbp), %ecx, the load of x before
   andl $7, orl $7 and xorl $7, and movsbl -5(%rbp), %ecx; addl $5,
   %ecx, and no mov of a constant; in commuted_register movl %ebx, %eax;
   addl $5, %eax, imull $4, %ebx, %ecx, which reads ebx, and the rest on
   a copy of ebx; in commuted_call the call, then addl $5, %eax, and no
   spill; in commuted_into imull $4, -12(%rbp), %ebx, then addl $5,
   %ebx alone; testl $1, -4(%rbp) then je in commuted_bit; and in
   commuted_char_register movl %ebx, %r12d; addl $5, %r12d, the
   operation on the variable's register.  It must show the constant
   first where it keeps the order: movabsq $4294967296, %rax; addq
   -8(%rbp), %rax in commuted_wide; and in not_commuted movl $5, %esi;
   subl -4(%rbp), %esi, movl $40, %eax before cltd and movl $1, %eax
   before the count's load.  It must show the index multiply with n its
   source and no load of n: movq -8(%rbp), %rax; imulq $4, -16(%rbp),
   %rcx; subq %rcx, %rax in index_sub, imulq $12, -16(%rbp), %rcx before
   addq %rcx, %rax in index_row, imulq $12, -16(%rbp), %rax then addq
   -8(%rbp), %rax in index_first, and imulq $4, -16(%rbp), %rax; subq
   %rax, -8(%rbp) in index_compound; the loads kept beside them, movslq
   -12(%rbp), %rcx; imulq $4, %rcx, %rcx in index_int and movq
   -16(%rbp), %rcx; leaq (%rax,%rcx,4), %rax in index_scaled; the load
   the widening, movslq -4(%rbp), %rax in widen_signedness and movl
   -4(%rbp), %eax alone in widen_unsigned, movsbq -1(%rbp), %rax alone
   in widen_char and widen_byte, and movq -8(%rbp), %rax;
   movslq %eax, %rax in widen_narrowed; cmpl %ebx, -12(%rbp) before jge
   and before setne in compare_register, and for x == r + 1 x's load and
   cmpl %ecx, %eax; movq -8(%rbp), %rax; addq $12, %rax in
   constant_index and constant_index_right alike, movq -16(%rbp), %rbx;
   addq $12, %rbx; addq $12, %rbx in constant_index_into, and in
   constant_index_wide the constant held first, movq $1073741824, %rax,
   then leaq (%rcx,%rax,4), %rax.

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
   4 * x as x * 4 is, the load before mov w2, #4, and mov x2, #1000 for
   the long.  It must show the same instructions in commuted and
   commuted_right: the load, then add w0, w0, #5, the load before
   mov w2, #4 and mul w1, w1, w2, and w1, w1, #0x7, orr and eor the
   same, and ldrb then add w1, w1, #5; in commuted_register
   add w0, w19, #5, mov w1, #4 then mul w1, w19, w1, and
   and w1, w19, #0x7 and the rest on w19; in commuted_call bl f, then
   add w0, w0, #5, and no spill; in commuted_into the load,
   mov w1, #4 and mul w19, w0, w1, then add w19, w19, #5;
   and w0, w0, #0x1 then cbz in commuted_bit; the load before
   mov x1, #4294967296 and add x0, x0, x1 in commuted_wide, the
   constant held on the right; add w20, w19, #5 in
   commuted_char_register; and in not_commuted the constant first,
   mov w0, #5 before the load and sub w0, w0, w1, mov w1, #40 before
   sdiv and mov w1, #1 before lsl.  It must show the rows of the pointer
   arithmetic, the casts and the comparison on registers after the
   loads: sub x0, x0, x1, lsl #2 in index_sub and sub x1, x1, x0, lsl #2
   in index_compound, mov x2, #12 and mul in index_row and index_first,
   sub x0, x0, w1, sxtw #2 in index_int; ldrsw x0, [x29, #16] alone in
   widen_signedness and ldr w0, [x29, #16] alone in widen_unsigned,
   ldrb w0, [x29, #16] alone in widen_char, the target's char unsigned,
   and ldrsb x0, [x29, #16] alone in widen_byte, ldr x0 then sxtw x0,
   w0 in widen_narrowed; ldr w0 then cmp w0, w19 in
   compare_register; add x0, x0, #12 after the load in constant_index
   and constant_index_right, add x19, x19, #12 then add x19, x19, #12 in
   constant_index_into, and mov x0, #4294967296 before the load in
   constant_index_wide. */
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

int
commuted(int x, char c)
{
        return (5 + x) - (4 * x) - (7 & x) - (7 | x) - (7 ^ x) - (5 + c);
}

int
commuted_right(int x, char c)
{
        return (x + 5) - (x * 4) - (x & 7) - (x | 7) - (x ^ 7) - (c + 5);
}

int
commuted_register(int a)
{
        register int x = a;

        return (5 + x) - (4 * x) - (7 & x) - (7 | x) - (7 ^ x);
}

int
commuted_call(void)
{
        return 5 + f();
}

int
commuted_into(int a)
{
        register int r;

        r = 4 * a;
        r = 5 + r;
        return r;
}

int
commuted_bit(int x)
{
        if (1 & x)
                return 1;
        return 0;
}

long
commuted_wide(long l)
{
        return 0x100000000 + l;
}

int
commuted_char_register(char c)
{
        register char rc = c;
        register int r;

        r = 5 + rc;
        return r;
}

int
not_commuted(int x)
{
        return (5 - x) + (40 / x) + (1 << x);
}

int *
index_sub(int *p, long n)
{
        return p - n;
}

int (*index_row(int (*m)[3], long n))[3]
{
        return m + n;
}

int (*index_first(int (*m)[3], long n))[3]
{
        return n + m;
}

int *
index_compound(int *p, long n)
{
        p -= n;
        return p;
}

int *
index_int(int *p, int i)
{
        return p - i;
}

int *
index_scaled(int *p, long n)
{
        return p + n;
}

long
widen_signedness(unsigned u)
{
        return (long) (int) u;
}

unsigned long
widen_unsigned(int i)
{
        return (unsigned long) (unsigned) i;
}

long
widen_narrowed(unsigned long ul)
{
        return (long) (int) ul;
}

int
compare_register(int x, int a)
{
        register int r = a;

        if (x < r)
                return x != r;
        return x == r + 1;
}

int *
constant_index(int *p)
{
        return 3 + p;
}

int *
constant_index_right(int *p)
{
        return p + 3;
}

int *
constant_index_into(int *q)
{
        register int *p;

        p = 3 + q;
        p = 3 + p;
        return p;
}

int *
constant_index_wide(int *p)
{
        return 0x40000000 + p;
}

long
widen_char(char c)
{
        return (long) (char) c;
}

long
widen_byte(unsigned char uc)
{
        return (long) (signed char) uc;
}
