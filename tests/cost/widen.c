/* backend.md 6.3 and aarch64.md 6.3, (T) a on an object in memory: the
   load of a signed object extends to the cast's width, and the cast
   writes nothing after it.  One access, at the object's width.

   On x86-64 the listing must show signed_char() loading c with
   movsbq -1(%rbp), %rax, short_() s with movswq -2(%rbp), %rax and
   int_() i with movslq -4(%rbp), %rax, each one instruction;
   char_() movsbq as signed_char(), plain char being signed on this
   target; unsigned_char() movzbl -1(%rbp), %eax, which leaves the
   value valid to 64 bits, and nothing after it; defined_() the
   PC-relative movslq defined(%rip), %rax; declared_() the GOT's load
   into rax, then movslq (%rax), %rax; pointer() p's load, then
   movswq (%rax), %rax; element() p's load, then movslq 8(%rax), %rax;
   and indexed() the index loaded with movsbq -9(%rbp), %rcx, then
   movl (%rax,%rcx,4), %eax.

   On aarch64 the listing must show signed_char() loading c with
   ldrsb x0, [x29, #16], short_() s with ldrsh x0, [x29, #16] and
   int_() i with ldrsw x0, [x29, #16], each one instruction; char_()
   ldrb w0, [x29, #16] and nothing after it, plain char being unsigned
   on this target, as unsigned_char() is; defined_() adrp x16, defined
   and ldrsw x0, [x16, :lo12:defined]; declared_() the GOT's two
   instructions into x0, then ldrsw x0, [x0]; pointer() p's load, then
   ldrsh x0, [x0]; element() p's load, then ldrsw x0, [x0, #8]; and
   indexed() the index loaded with ldrsb w1, [x29, #24] and read by
   ldr w0, [x0, w1, sxtw #2], the form extending it. */
int defined;
extern int declared;

long
signed_char(signed char c)
{
        return c;
}

long
short_(short s)
{
        return s;
}

long
int_(int i)
{
        return i;
}

long
char_(char c)
{
        return c;
}

long
unsigned_char(unsigned char c)
{
        return c;
}

long
defined_(void)
{
        return defined;
}

long
declared_(void)
{
        return declared;
}

long
pointer(short *p)
{
        return *p;
}

long
element(int *p)
{
        return p[2];
}

int
indexed(int *p, signed char i)
{
        return p[i];
}
