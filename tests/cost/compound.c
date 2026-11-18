/* backend.md 6.3 and 6.6 and aarch64.md 6.3 and 6.6, x op= e: e first,
   then x.  In the effect context the operation is on the object where
   the machine has that form; otherwise, and in a value context, it is
   the load row, the op row and the store row.

   On x86-64 the listing must show compound() loading b and adding it to
   x's slot with addl %eax, -12(%rbp); narrow() adding the low byte of b
   to c's slot with addb %al, -9(%rbp); and in_value(), whose value is
   returned, loading b, the right operand, first, then c with movsbl
   into eax, adding b in int with addl %ecx, %eax and storing the low
   byte back with movb %al.

   On aarch64, which has no memory form, the listing must show
   compound() loading b into w0, then x into w1, add w1, w1, w0 and
   str w1 back to x's slot; narrow() the same on c, ldrb w1, the add and
   strb w1; and in_value() the same loads, then add w0, w1, w0, the
   operation written into the return's w0, strb w0 and uxtb w0, w0, the
   char value widened for the return.

   The value of an unsigned one at its own width, widened to long, is
   the operation's 32-bit write, which clears the upper half:
   unsigned_value() *p += e, unsigned_divided() *p /= e,
   unsigned_shifted() *p >>= n, unsigned_step() ++*p, unsigned_global()
   counter -= e on a global and unsigned_slot() u *= e on a local;
   beside them unsigned_postfix() (*p)++, whose old value is the load,
   unsigned_wide() *p += l on a long l, an operation in long narrowed
   by the store, unsigned_narrow() *p += e on an unsigned short,
   re-extended, signed_value() *p += e on an int, sign-extended, and
   unsigned_plain() *p = l, the long's value narrowed.

   On x86-64 the listing must show unsigned_value() addl %edx, %eax and
   movl %eax, (%rcx) with nothing after it, unsigned_divided() divl
   %esi and the store, unsigned_shifted() shrl %cl, %eax and the store,
   unsigned_step() addl $1, %eax and the store, unsigned_global() subl
   %ecx, %eax and movl %eax, counter(%rip), and unsigned_slot() imull
   %ecx, %eax and movl %eax, -12(%rbp), each with nothing after the
   store; unsigned_postfix() movl %edx, %eax, the old value's copy;
   unsigned_wide() addq %rdx, %rax, the store and movl %eax, %eax;
   unsigned_narrow() movw %ax, (%rcx) and movzwq %ax, %rax;
   signed_value() the store and movslq %eax, %rax; and unsigned_plain()
   movl %eax, (%rcx) and movl %eax, %eax.

   On aarch64 the listing must show unsigned_value() add w0, w2, w0 and
   str w0, [x1] with nothing after it, unsigned_divided() udiv w0, w2,
   w0, unsigned_shifted() lsr w0, w2, w0, unsigned_step() add w0, w0,
   #1, unsigned_global() sub w0, w1, w0 and the store through x16, and
   unsigned_slot() mul w0, w1, w0 and str w0, [x29, #24], each with
   nothing after the store; unsigned_postfix() the load into w0 and add
   w2, w0, #1; unsigned_wide() add x0, x2, x0, str w0, [x1] and mov w0,
   w0; unsigned_narrow() strh w0, [x1] and uxth w0, w0; signed_value()
   str w0, [x1] and sxtw x0, w0; and unsigned_plain() str w0, [x1] and
   mov w0, w0. */
int compound(int a, int b)
{
    int x = a;
    x += b;
    return x;
}

int narrow(int a, int b)
{
    char c = a;
    c += b;
    return c;
}

int in_value(int a, int b)
{
    char c = a;
    return c += b;
}

unsigned counter;

long
unsigned_value(unsigned *p, unsigned e)
{
        return *p += e;
}

long
unsigned_divided(unsigned *p, unsigned e)
{
        return *p /= e;
}

long
unsigned_shifted(unsigned *p, int n)
{
        return *p >>= n;
}

long
unsigned_step(unsigned *p)
{
        return ++*p;
}

long
unsigned_global(unsigned e)
{
        return counter -= e;
}

long
unsigned_slot(unsigned a, unsigned e)
{
        unsigned u = a;

        return u *= e;
}

long
unsigned_postfix(unsigned *p)
{
        return (*p)++;
}

long
unsigned_wide(unsigned *p, long l)
{
        return *p += l;
}

long
unsigned_narrow(unsigned short *p, unsigned e)
{
        return *p += e;
}

long
signed_value(int *p, int e)
{
        return *p += e;
}

long
unsigned_plain(unsigned *p, long l)
{
        return *p = l;
}
