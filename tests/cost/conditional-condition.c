/* backend.md 7.1 and aarch64.md 7.1, e ? a : b as a condition: jump
   code, as a && b is, where an arm is a truth value: the condition
   jumps to the second arm when it fails, and each arm is a condition
   of the same jump, the first followed by a jump past the second, with
   no join.  Two arms neither of which is a truth value are joined and
   the join tested once.  joined() tests two ints in memory in an if;
   compared() an arm that is a comparison and one that is an int in
   memory; called() an arm that is a call; negated() the second under
   !; looped() the second as a while's condition; both() two
   comparisons; held() the second as the condition of a conditional
   added to d, which is held across its paths; written() the same
   beside a register variable's read, which an arm writes; converted()
   the second with an unsigned second arm, which converts the
   comparison to unsigned; narrow() with a char second arm, which is
   promoted; registers() two register variables' reads, which on
   aarch64 alone are arms tested in place.  Each arm is tested without
   its conversion.

   On x86-64 the listing must show in joined() c compared in its slot,
   cmpl $0, -4(%rbp), and je to the second arm, each arm loaded into
   eax, a jmp past the second, and the join's cmpl $0, %eax; je; in
   called() the call's result and the second arm's load joined in eax
   the same; in compared() the comparison's cmpl -12(%rbp), %eax and
   jge to the label past the body, with no setl, a jmp past the second
   arm, and the second arm compared in its slot, cmpl $0, -16(%rbp);
   je, with no join; in negated() the same with the jumps' senses
   inverted, jl and jne; in looped() the same as compared() at the
   loop's head; in both() jge and jle, one per arm; in held() d loaded
   and spilled, movq %rax, -24(%rbp), before the first jump, the outer
   conditional's movl $1, %ecx and movl $2, %ecx, and movq -24(%rbp),
   %rax; addl %ecx, %eax after the join; and in written() r saved,
   movq %rbx, -32(%rbp), before the first jump, the first arm's
   assignment into ebx and its cmpl -20(%rbp), %ebx, the second arm's
   cmpl $0, %ebx, and the saved read reloaded after the join, movq
   -32(%rbp), %rax; in converted() the same as compared(), the
   comparison's jge and the unsigned arm's cmpl $0, -16(%rbp); and in
   narrow() the char compared in its slot at its width, cmpb $0,
   -13(%rbp); and in registers() the arms joined in eax, movl %ebx,
   %eax and movl %r12d, %eax, and the join's cmpl $0, %eax; je.

   On aarch64 the listing must show in joined() and called() the arms
   joined in w0 and the join's one cbz w0; in compared() the
   comparison's cmp w0, w1 and b.ge to the label past the body, with no
   cset, a b past the second arm, and the second arm's ldr w0, [x29,
   #28]; cbz w0, with no join; in negated() b.lt and cbnz; in looped()
   the same as compared() at the loop's head; in both() b.ge and b.le;
   in held() d loaded and spilled, str x0, [x29, #32], before the first
   branch, and ldr x1, [x29, #32]; add w0, w1, w0 after the outer
   conditional's join; and in written() r's register saved, str x19,
   [x29, #40], the first arm's ldr w19 and cmp w19, w0, the second
   arm's cbz w19, and ldr x1, [x29, #40] after the join; in converted()
   the same as compared(); in narrow() the char's ldrb w0, [x29,
   #28]; cbz w0; and in registers() each arm tested in its register,
   cbz w19 and cbz w20, with no join. */
void g(void);
int f(void);

void
joined(int c, int x, int y)
{
        if (c ? x : y)
                g();
}

void
compared(int c, int a, int b, int d)
{
        if (c ? a < b : d)
                g();
}

void
called(int c, int y)
{
        if (c ? f() : y)
                g();
}

void
negated(int c, int a, int b, int d)
{
        if (!(c ? a < b : d))
                g();
}

void
looped(int c, int a, int b, int d)
{
        while (c ? a < b : d)
                g();
}

void
both(int c, int a, int b, int d)
{
        if (c ? a < b : a > d)
                g();
}

int
held(int c, int a, int b, int d)
{
        return d + ((c ? a < b : d) ? 1 : 2);
}

int
written(int c, int b, int d)
{
        register int r = d;

        return r + ((c ? (r = b) < d : r) ? 1 : 2);
}

void
converted(int c, int a, int b, unsigned u)
{
        if (c ? a < b : u)
                g();
}

void
narrow(int c, int a, int b, char d)
{
        if (c ? a < b : d)
                g();
}

void
registers(int c, int x, int y)
{
        register int a = x;
        register int b = y;

        if (c ? a : b)
                g();
}
