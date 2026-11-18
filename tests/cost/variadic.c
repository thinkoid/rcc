/* shape.md chapter 5, backend.md chapter 5, aarch64.md 5.2 and G19:
   variadic definitions, 7.16.  va_start builds the list from the
   function's save area, which the prologue fills with every argument
   register, general and vector, and the address of the arguments on
   the stack; va_arg reads the next 8-byte slot, the save area's while
   a register is left and then the stack's, at the type's width;
   va_copy is a record's assignment; va_end is the list's address,
   discarded; a list is handed to the C library, on x86-64 as the
   address of its record and on aarch64 as the address of a copy the
   caller makes; and a list with a side effect, *lists++ in stepped,
   is evaluated once, its address assigned to a temporary the tree
   reads it through.

   On x86-64, each function's save area is the slot of 184 bytes after
   its named parameter's, at -192(%rbp): after the parameter's own
   store, movl %edi, -4(%rbp) in sum and twice and movq %rdi, -8(%rbp)
   in print, six movq of %rdi to %r9 to -192 up to -152 and eight
   movaps of %xmm0 to %xmm7 to -144 up to -32, with no test of %al,
   then leaq 16(%rbp), %rax and movq %rax, -16(%rbp), the area's 176.
   va_start is four stores into the list at -216, movl $8, -216(%rbp)
   and movl $48, -212(%rbp) for one named parameter (print's 8 and
   48 alike), the address at 176 loaded and stored at -208, and
   leaq -192(%rbp) stored at -200.  va_arg is cmpl $40, -216(%rbp)
   and ja to the stack's path; the area's, addl $8, -216(%rbp), then
   the area and the offset less 8 added, movq -200(%rbp), %rax, movl
   -216(%rbp), %ecx, subl $8, %ecx, addq %rcx, %rax; the stack's,
   the address stepped by addq $8 and stored, then less 8; and the
   load of the type's width through %rax, movq (%rax), %rax in sum
   and movl (%rax), %eax in twice.  va_copy is three 8-byte units,
   movq -216(%rbp), %rax; movq %rax, -240(%rbp) and on.  va_end is the
   list's leaq, discarded.  print hands vprintf the list's address,
   leaq -216(%rbp), %rax, from its argument slot into %rsi.  In
   stepped, the pointer is stepped once for each va_arg, movq
   -8(%rbp), %rax; movq %rax, %rcx; addq $24, %rax; movq %rax,
   -8(%rbp), and the list's address stored in the temporary, movq %rcx,
   -24(%rbp), whose slot follows the parameters' and precedes the
   body's own; each read of the list then loads the temporary, movq
   -24(%rbp), %rax, and reaches the member through it, cmpl $40,
   (%rax), addl $8, (%rax), movq 16(%rax), %rax.

   On aarch64 each definition stores the area first, right after
   the frame record, at [x29, #16]: x0 to x7 by stp x0, x1, [x29,
   #16] to stp x6, x7, [x29, #64], q0 to q7 by stp q0, q1, [x29,
   #80] to stp q6, q7, [x29, #176], and at 192 the address of the
   stacked arguments, the caller's sp, add x16, x29, #272; str x16,
   [x29, #208] in sum(); then the named parameter's own store, str w0,
   [x29, #216].  va_start writes the list's members from it: __stack
   loaded from [x29, #208], __gr_top add x0, x29, #80, __vr_top add x0,
   x29, #208, __gr_offs mov w0, #-56, past the one named parameter,
   and __vr_offs mov w0, #-128.  va_arg tests __gr_offs, cmp w0, #0;
   b.ge, steps it by 8 and reads at __gr_top plus the old offset, add
   x0, x0, w1, sxtw, or steps __stack by 8 and reads below it, then ldr
   x0, [x0] in sum() and ldr w0, [x0] in twice().  va_copy in twice()
   is the copy of the list's 32 bytes, four ldr x17 and str x17 units.
   va_end is the list's address, add x0, x29, #224, discarded.  print()
   hands the list to vprintf as the address of a copy: the four units
   into a slot of the call's own after the first argument's, ldr x17,
   [x29, #224]; str x17, [x29, #272] and on, then ldr x0, [x29, #264];
   add x1, x29, #272; bl vprintf.  In stepped() the pointer is stepped
   once for each va_arg, ldr x0, [x29, #16]; add x1, x0, #32; str x1,
   [x29, #16], and the list's address stored in the temporary, str x0,
   [x29, #32], whose slot follows the parameters' and precedes the
   body's own; each read of the list then loads the temporary and
   reaches the member through it, ldr x0, [x29, #32]; ldr w0, [x0,
   #24]. */
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

long
stepped(va_list *lists, int count)
{
        long s = 0;

        while (count-- > 0)
                s += va_arg(*lists++, long);

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
