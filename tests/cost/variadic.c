/* shape.md chapter 5, backend.md chapter 5 and G19: variadic
   definitions, 7.16.  va_start builds the list from the function's
   save area, which the prologue fills with every argument register,
   general and vector, and the address of the arguments on the stack;
   va_arg reads the next 8-byte slot, the save area's while a register
   is left and then the stack's, at the type's width; va_copy is a
   record's assignment; va_end is the list's address, discarded; and a
   list is handed to the C library, on x86-64 as the address of its
   record and on aarch64 as the address of a copy the caller makes.

   On x86-64, each function's save area is the slot of 184 bytes after
   its named parameter's, at -192(%rbp): after the parameter's own
   store, movl %edi, -4(%rbp) in sum and twice and movq %rdi, -8(%rbp)
   in print, six movq of %rdi to %r9 to -192 up to -152 and eight
   movaps of %xmm0 to %xmm7 to -144 up to -32, with no test of %al,
   then leaq 16(%rbp), %rax and movq %rax, -16(%rbp), the area's 176.
   va_start is four stores into the list at -216, movl $8, -216(%rbp)
   and movl $48, -212(%rbp) for one named parameter (print's 8 and 48
   alike), the address at 176 loaded and stored at -208, and leaq
   -192(%rbp) stored at -200.  va_arg is cmpl $40, -216(%rbp) and ja
   to the stack's path; the area's, addl $8, -216(%rbp), then the area
   and the offset less 8 added, movq -200(%rbp), %rax, movl
   -216(%rbp), %ecx, subl $8, %ecx, addq %rcx, %rax; the stack's, the
   address stepped by addq $8 and stored, then less 8; and the load of
   the type's width through %rax, movq (%rax), %rax in sum and movl
   (%rax), %eax in twice.  va_copy is three 8-byte units, movq
   -216(%rbp), %rax; movq %rax, -240(%rbp) and on.  va_end is the
   list's leaq, discarded.  print hands vprintf the list's address,
   leaq -216(%rbp), %rax, from its argument slot into %rsi. */
#include <stdarg.h>
#include <stdio.h>

long
sum(int count, ...)
{
        va_list ap;
        long s = 0;

        va_start(ap, count);
        while (count-- > 0)
                s += va_arg(ap, long);
        va_end(ap);

        return s;
}

int
twice(int count, ...)
{
        va_list ap, again;
        int s = 0;

        va_start(ap, count);
        va_copy(again, ap);
        for (int n = count; n > 0; --n)
                s += va_arg(ap, int) + va_arg(again, int);
        va_end(again);
        va_end(ap);

        return s;
}

int
print(const char *form, ...)
{
        va_list ap;
        int n;

        va_start(ap, form);
        n = vprintf(form, ap);
        va_end(ap);

        return n;
}
