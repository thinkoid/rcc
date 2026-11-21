/* cost.md's _Bool row: a _Bool is a byte, read and written as an
   unsigned char is (G21), and a conversion to it is the parser's tree
   (_Bool) (E != 0), a comparison with 0 and a set the source does not
   write, which a value already 0 or 1 and a _Bool do not need.  Each
   operator that stores into a _Bool stores the conversion of its
   result, E1 = (_Bool) (E1 op (E2)), and a postfix step holds the
   value it yields in a temporary.

   On x86-64 the listing must show from_int() as cmpl $0, i(%rip);
   setne %al; movzbl %al, %eax; movb %al, b(%rip), from_long() and
   from_pointer() the same with cmpq on all 64 bits, from_constant()
   the one movb $1, b(%rip), and from_comparison() setl and the store
   with no second comparison; copied(), passed() and returned() the
   byte as it is, movzbl c(%rip), %eax, and no comparison; read() the
   plain byte load movzbl b(%rip), %eax and condition() cmpb $0,
   b(%rip); each compound assignment the byte's load, the operation on
   the int, then cmpl $0, %eax; setne %al before the store, rem() on
   %edx, the remainder; pre_inc() and pre_dec() the same with addl $1
   and subl $1; and post_inc() and post_dec() the byte held in
   -1(%rbp), the operation on it, the store, and movzbl -1(%rbp), %eax
   as the value.

   On aarch64 the listing must show from_int() as cmp w0, #0; cset w0,
   ne; strb w0 after the load, from_long() and from_pointer() cmp x0,
   #0, from_constant() mov w0, #1; strb, and from_comparison() cset w0,
   lt and the store with no second comparison; copied(), passed() and
   returned() ldrb and no comparison; read() the plain ldrb and
   condition() ldrb; cbz; each compound assignment the operation on the
   ldrb's value, then cmp w0, #0; cset w0, ne before the strb, rem()
   after its msub; pre_inc() and pre_dec() the same with add w0, w0, #1
   and sub w0, w0, #1; and post_inc() and post_dec() the byte held in
   [x29, #16], the operation on it, the store, and ldrb w0, [x29, #16]
   as the value. */
_Bool b;
_Bool c;
int i;
long l;
char *p;

void
from_int(void)
{
        b = i;
}

void
from_long(void)
{
        b = l;
}

void
from_pointer(void)
{
        b = p;
}

void
from_constant(void)
{
        b = 5;
}

void
from_comparison(void)
{
        b = i < l;
}

void
copied(void)
{
        b = c;
}

void take(_Bool x);

void
passed(void)
{
        take(c);
}

_Bool
returned(void)
{
        return c;
}

int
read(void)
{
        return b + 1;
}

int
condition(void)
{
        if (b)
                return 3;

        return 4;
}

void
add(void)
{
        b += i;
}

void
sub(void)
{
        b -= i;
}

void
and(void)
{
        b &= i;
}

void
or(void)
{
        b |= i;
}

void
xor(void)
{
        b ^= i;
}

void
shl(void)
{
        b <<= i;
}

void
shr(void)
{
        b >>= i;
}

void
mul(void)
{
        b *= i;
}

void
div(void)
{
        b /= i;
}

void
rem(void)
{
        b %= i;
}

int
pre_inc(void)
{
        return ++b;
}

int
pre_dec(void)
{
        return --b;
}

int
post_inc(void)
{
        return b++;
}

int
post_dec(void)
{
        return b--;
}
