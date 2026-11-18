/* backend.md 7.2 and aarch64.md 7.2, a conditional whose value is
   dropped: two void operands, 6.5.16p3, p6, or two of a value in an
   expression statement.  It is its effect alone: the condition's jump
   to the second arm, the first arm for its effect, the jump past the
   second, the second arm for its effect, and no join and no value.
   pick() chooses between two void calls, dropped() between two calls
   whose results are dropped, nested() takes a void conditional as an
   arm, and stored() two constants assigned under casts to void.

   On x86-64 the listing must show pick() as cmpl $0, -4(%rbp);
   je .Lpick.2; call f@PLT; jmp .Lpick.3; .Lpick.2: call g@PLT;
   .Lpick.3:, with nothing between the second call and the label;
   dropped() the same around call h@PLT and call k@PLT, whose results
   nothing moves; nested() the inner conditional's own jumps inside the
   first arm, then the first arm's jmp .Lnested.3 after the inner
   label; and stored() the store of each constant, movl $1, x(%rip)
   and movl $2, x(%rip), each the one store of an effect.

   On aarch64 the listing must show pick() as ldr w0, [x29, #16];
   cbz w0, .Lpick.2; bl f; b .Lpick.3; .Lpick.2: bl g; .Lpick.3:, with
   nothing between the second call and the label; dropped() the same
   around bl h and bl k; nested() the inner conditional's own branches
   inside the first arm, then the first arm's b .Lnested.3 after the
   inner label; and stored() each constant stored, mov w0, #1 and
   mov w0, #2, each before its str w0, [x16, :lo12:x]. */
void f(void);
void g(void);
int h(void);
int k(void);
int x;

void
pick(int c)
{
        c ? f() : g();
}

void
dropped(int c)
{
        c ? h() : k();
}

void
nested(int c, int d)
{
        c ? (d ? f() : g()) : f();
}

void
stored(int c)
{
        c ? (void) (x = 1) : (void) (x = 2);
}
