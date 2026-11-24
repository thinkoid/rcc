/* shape.md chapter 5, a va_arg whose list is reached through a
   volatile pointer: computing the list's address reads the pointer, a
   side effect, 5.1.2.4p2, which the source writes once, so the list is
   taken as one with a side effect, as *lists++ in variadic's stepped:
   its address is assigned once to a temporary, and the tree reads each
   member through the temporary.  list is loaded once, where va_arg
   stands; va_start and va_end name ap itself.

   On x86-64, list is the slot at -232(%rbp), ap the record at
   -224(%rbp) and the temporary the slot at -200(%rbp).  list is
   stored once, leaq -224(%rbp), %rax; movq %rax, -232(%rbp), and
   loaded once, movq -232(%rbp), %rax; movq %rax, -200(%rbp), after
   va_start's four stores; each read of a member of the list then
   loads the temporary, movq -200(%rbp), %rax, and reaches the member
   through it, cmpl $40, (%rax), addl $8, (%rax), movq 16(%rax), %rax.

   On aarch64, list is the slot at [x29, #264], ap the record at [x29,
   #232] and the temporary the slot at [x29, #224].  list is stored
   once, add x0, x29, #232; str x0, [x29, #264], and loaded once, ldr
   x0, [x29, #264]; str x0, [x29, #224], after va_start's stores; each
   read of a member then loads the temporary, ldr x0, [x29, #224], and
   reaches the member through it, ldr w0, [x0, #24] and ldr x0, [x0,
   #8]. */
#include <stdarg.h>

int
first(int n, ...)
{
        va_list ap;
        va_list *volatile list = &ap;
        int value;

        va_start(ap, n);
        value = va_arg(*list, int);
        va_end(ap);

        return value;
}
