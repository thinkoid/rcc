/* GNU C's a ?: b, the conditional whose middle operand is left out:
   a ? a : b with a evaluated once (cost.md 2.1, 2.2).  The condition's
   value is the first arm's, so it is computed into the register the
   arms join in and tested there, a jump past the second arm when it is
   not 0, and the second arm is written into the same register: no
   slot holds a and nothing reads it twice.  in_memory() stores x or y,
   two ints in memory, a consumer that names no register; called()
   returns f() or g(); pointer() returns p or q; narrow() returns c, a
   char, or i, the char converted to int before its test; condition()
   tests a ?: b in an if; into_register() assigns a ?: b to a register
   variable; returned() returns x or y; discarded() calls f() and, when
   its result is 0, g(), the value dropped.

   On x86-64 the listing must show in_memory() as movl -4(%rbp), %eax;
   cmpl $0, %eax; jne .Lin_memory.2; movl -8(%rbp), %eax;
   .Lin_memory.2: and the store movl %eax, h(%rip); called() as call
   f@PLT; cmpl $0, %eax; jne .Lcalled.2; call g@PLT, with no spill of
   f()'s result; pointer() the same at 64 bits, movq -8(%rbp), %rax;
   cmpq $0, %rax; narrow() the extending load movsbl -1(%rbp), %eax
   before cmpl $0, %eax; condition() the join, then cmpl $0, %eax;
   je .Lcondition.2 for the if; into_register() the condition loaded
   into the variable, movl -12(%rbp), %ebx; cmpl $0, %ebx; jne, and
   the second arm's movl -16(%rbp), %ebx, with no join in another
   register; returned() as in_memory() with nothing after the join but
   the jump to the epilogue; and discarded() as called() with no value
   after the label.

   On aarch64 the listing must show in_memory() as ldr w0, [x29, #16];
   cbnz w0, .Lin_memory.2; ldr w0, [x29, #20]; .Lin_memory.2: and the
   store str w0, [x16, :lo12:h]; called() as bl f; cbnz w0,
   .Lcalled.2; bl g; pointer() the same at 64 bits, cbnz x0; narrow()
   the load ldrb w0, [x29, #16], a char being unsigned there, before
   cbnz w0; condition() the join, then cbz w0, .Lcondition.2 for the
   if; into_register() ldr w19, [x29, #24]; cbnz w19 and the second
   arm's ldr w19, [x29, #28]; returned() as in_memory() with nothing
   after the join but the branch to the epilogue; and discarded() as
   called() with no value after the label. */
int h;

void
in_memory(int x, int y)
{
        h = x ?: y;
}

int f(void);
int g(void);

int
called(void)
{
        return f() ?: g();
}

char *
pointer(char *p, char *q)
{
        return p ?: q;
}

int
narrow(char c, int i)
{
        return c ?: i;
}

int
condition(int a, int b)
{
        if (a ?: b)
                return 1;

        return 0;
}

int
into_register(int a, int b)
{
        register int r;

        r = a ?: b;

        return r;
}

int
returned(int x, int y)
{
        return x ?: y;
}

void
discarded(void)
{
        f() ?: g();
}
