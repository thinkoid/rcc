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
   the conditions on an object in memory and a constant.  Beside them,
   char_operand, widened, compound, compound_value, shift_count,
   times_four and before_call keep the load: a char promoted, an int
   widened to long, a compound assignment's right operand for its
   effect and for its value, a shift's count, a multiply by a constant
   and a right operand that is a call, which may write the global
   loaded before it.

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
   before shll %cl in shift_count, the load and imull $4, %eax, %eax in
   times_four, and in before_call the load of defined before the call,
   its spill and its reload, then cmpl %eax, %ecx.

   On aarch64, a load-store machine with no instruction that takes a
   memory operand, the listing must show each operand loaded into a
   register, ldr w1, [x29, #36] for y, and each operation on registers,
   add w0, w0, w1 and the rest, sdiv w0, w0, w1, sdiv and msub in
   modulo, cmp w0, w1 before cset, and w0, w0, w1 before cbz in test;
   ldr then cmp w0, #5 in less_constant, cbz w0 in nonzero and
   nonzero_char, and w0, w0, #0x1 then cbz in bit. */
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
