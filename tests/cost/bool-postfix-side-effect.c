/* shape.md chapter 5, a postfix step of a _Bool through a place with a
   side effect, (*cursor++)++, its value used: the value is the _Bool's
   before the store, held in a temporary of its own, and the place's
   address, cursor's old value, is held in another, evaluated first, so
   cursor is stepped once: (a = cursor++, (b = *a, *a = (_Bool) (b +
   1)), b).

   On x86-64, the step of cursor, movq %rcx, -8(%rbp) into the address's
   temporary, movq -8(%rbp), %rax; movzbl (%rax), %eax; movb %al,
   -9(%rbp) into the value's, movq -8(%rbp), %rax for the store, addl
   $1, the conversion, movb %cl, (%rax), and the value movzbl -9(%rbp),
   %eax.

   On aarch64, the step of cursor, str x0, [x29, #16] into the address's
   temporary, ldr x0, [x29, #16]; ldrb w0, [x0]; strb w0, [x29, #24]
   into the value's, ldr x0, [x29, #16] for the store, add w1, w1, #1,
   cmp; cset, strb w1, [x0], and the value ldrb w0, [x29, #24]. */
_Bool *cursor;

int
take(void)
{
        return (*cursor++)++;
}
