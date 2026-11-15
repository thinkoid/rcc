/* backend.md chapter 10 and aarch64.md chapter 10, static: a function
   and an object of internal linkage, and a block's static object, each
   beside its twin of external linkage.  hidden() and shown() are one
   function, the first static; calls() calls each and takes the
   address of each; quiet and loud are one object, the first static,
   which get_quiet() and get_loud() read; next() steps count, a static
   object of its block, and next_total() steps total, a global.

   On x86-64 the listing must show the two of each pair with the same
   code and the same directives but for the .globl line, which hidden,
   quiet and next.count have none of: hidden() under .type
   hidden, @function alone, shown() under .globl shown too; calls()
   with call hidden@PLT and call shown@PLT, and each address
   PC-relative, leaq hidden(%rip), %rax and leaq shown(%rip), %rax;
   get_quiet() movl quiet(%rip), %eax and get_loud()
   movl loud(%rip), %eax; next() movl next.count(%rip), %eax,
   addl $1, %eax and movl %eax, next.count(%rip), the label of the
   block's object the function's name and its own, and next_total()
   the same with total(%rip).  The objects are in .bss, in the order of
   their definitions, quiet, loud, total and next.count, each under
   .type, .size and .p2align 2, loud and total under .globl too.

   On aarch64 the listing must show the same pairs and the same
   difference: calls() with bl hidden and bl shown, each address
   adrp x0, hidden and add x0, x0, :lo12:hidden; each object reached
   through x16, adrp x16, quiet and ldr w0, [x16, :lo12:quiet], and
   adrp x16, next.count with [x16, :lo12:next.count] for the block's
   object; the data as on x86-64. */
static int quiet;
int loud;

int total;

static int
hidden(int x)
{
        return x + 1;
}

int
shown(int x)
{
        return x + 1;
}

long
calls(int x)
{
        long a = (long) hidden;
        long b = (long) shown;

        return hidden(x) + shown(x) + a - b;
}

int
get_quiet(void)
{
        return quiet;
}

int
get_loud(void)
{
        return loud;
}

int
next(void)
{
        static int count;

        return ++count;
}

int
next_total(void)
{
        return ++total;
}
