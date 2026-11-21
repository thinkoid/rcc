/* GNU C's statement expression, ({ ... }): its statements run where it
   stands, each the statement it would be at a boundary of statements,
   and its value is its last expression's (shape.md chapter 5,
   backend.md 6.7).  It writes no instruction of its own.  The values
   held when it starts are saved as at a branch where its statements
   may move them, and left where they are elsewhere.  interrupts_off()
   is the form a program's macro writes as a statement, its value
   dropped; assigned() stores the value of ({ int t = f(); t + 1; });
   held() adds ({ g(); b; }) to a, held beside it; argument() passes
   ({ g(); 2; }) between two constants; declared() passes ({ int t =
   f(); t * 2; }), whose t is declared while the argument is evaluated;
   tested() branches on ({ g(); a; }); copied() returns the read of
   a register variable the block declares; and bare() and local() add
   ({ b; }) and ({ int t = b; t; }) to a, held beside them, whose
   statements move nothing.

   On x86-64 the listing must show interrupts_off() as
   addl $1, pending(%rip) and the value dropped as the statement 0;
   is, movl $0, %eax; assigned() as call f@PLT; movl %eax, -8(%rbp);
   movl -8(%rbp), %eax; addl $1, %eax; movl %eax, -4(%rbp), as the
   twin { int t = f(); x = t + 1; } writes it; held() as a's load,
   movl -4(%rbp), %eax, spilled before the block by
   movq %rax, -16(%rbp), then call g@PLT, b's load and
   addl -16(%rbp), %eax; argument() as movl $1, -4(%rbp), call g@PLT,
   movl $2, %eax; movl %eax, -8(%rbp) and movl $3, -12(%rbp) before the
   loads into the argument registers; declared() with t's slot,
   -12(%rbp), after the second argument's, -8(%rbp), which the call
   took before the argument was evaluated; tested() as call g@PLT, then
   cmpl $0, -4(%rbp); je; copied() as call f@PLT; movl %eax, %ebx and
   the read copied, movl %ebx, %eax; bare() as a + (b) is,
   movl -4(%rbp), %eax; addl -8(%rbp), %eax, with no spill; and local()
   the same with the declaration's movl -8(%rbp), %ecx;
   movl %ecx, -12(%rbp) between, t's slot the addl's operand.

   On aarch64 the listing must show interrupts_off() as pending's load,
   add w0, w0, #1 and its store, then mov w0, #0; assigned() as bl f;
   str w0, [x29, #20]; ldr w0, [x29, #20]; add w0, w0, #1;
   str w0, [x29, #16]; held() as a's load, ldr w0, [x29, #16], spilled
   before the block by str x0, [x29, #24], then bl g, b's load, the
   reload ldr x1, [x29, #24] and add w0, w1, w0; argument() as
   mov w0, #1; str w0, [x29, #16], bl g, mov w0, #2;
   str w0, [x29, #20] and the third argument's store before the loads
   into x0 to x2; declared() with t's slot, [x29, #24], after the second
   argument's, [x29, #20]; tested() as bl g, then a's load and
   cbz w0; copied() as bl f; mov w19, w0 and the read copied,
   mov w0, w19; bare() as a + (b) is, ldr w0, [x29, #16];
   ldr w1, [x29, #20]; add w0, w0, w1, with no spill; and local() the
   same with the declaration's str w1, [x29, #24] and the load
   ldr w1, [x29, #24] before the add. */
int pending;

int f(void);
void g(void);
int three(int, int, int);

void
interrupts_off(void)
{
        ({
                pending++;
                0;
        });
}

int
assigned(void)
{
        int x;

        x = ({
                int t = f();
                t + 1;
        });

        return x;
}

int
held(int a, int b)
{
        return a + ({
                       g();
                       b;
               });
}

int
argument(void)
{
        return three(1, ({
                             g();
                             2;
                     }),
                     3);
}

int
declared(void)
{
        return three(1, ({
                             int t = f();
                             t * 2;
                     }),
                     3);
}

int
tested(int a)
{
        if (({
                    g();
                    a;
            }))
                return 1;

        return 0;
}

int
copied(void)
{
        return ({
                register int r = f();
                r;
        });
}

int
bare(int a, int b)
{
        return a + ({ b; });
}

int
local(int a, int b)
{
        return a + ({
                       int t = b;
                       t;
               });
}
