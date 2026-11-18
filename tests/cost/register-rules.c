/* backend.md chapter 4 and aarch64.md chapter 4, the register rules no
   other row shows: compound_in_place(), r += 5 and ++r on the register
   in place; a narrow register variable, narrow_store() a call's result
   stored by the extending move, narrow_step() c++ re-extended after
   the step, and narrow_unwritten() an unwritten one extended at its
   declaration, as narrow_store()'s c is too; the copy of a held read
   before its variable is written (G13), held_step() i + i++, and
   before the paths of &&, || and ?: (backend.md 6.7), held_and(),
   held_or() and held_conditional(), i + (b && i++) and the same with
   || and ?:; stacked() a register parameter past the argument
   registers, the seventh and the ninth, s and k, and mixed() the
   moves and the stores of register and ordinary parameters in
   parameter order; epilogue() a function with a push and a sub of its
   own; and sixth() a sixth variable live beside five others, then a
   later block's variable t.

   On x86-64, compound_in_place() is addl $5, %ebx and addl $1, %ebx
   on the variable's register.  narrow_store() extends the unwritten c
   at its declaration, movsbl %bl, %ebx, and stores g's result by
   movsbl %al, %ebx; narrow_step() is addl $1, %ebx and movsbl %bl,
   %ebx after it; and narrow_unwritten() the one movsbl %bl, %ebx
   before the return's movl %ebx, %eax.  held_step() copies the held
   read and the step's old value, movl %ebx, %eax and movl %ebx, %ecx,
   before addl $1, %ebx.  held_and(), held_or() and
   held_conditional() store the variable's register before the first
   jump, movq %rbx, -24(%rbp), copy the read before the step on the
   path, movl %ebx, %ecx, and reload the saved read where the paths
   meet, movq -24(%rbp), %rax, before addl %ecx, %eax.  stacked()
   stores the six argument registers, then loads s and k from above
   the return address, movl 16(%rbp), %ebx and movl 32(%rbp), %r12d,
   and reads h in place, addl 24(%rbp), %eax.  mixed() is movl %edi,
   -20(%rbp), movl %esi, %ebx, movl %edx, -24(%rbp) and movl %ecx,
   %r12d, in that order.  epilogue(), whose prologue is pushq %rbx and
   subq $24, %rsp, writes leaq -8(%rbp), %rsp before popq %rbx and
   leave.  sixth() takes rbx, r12, r13, r14 and r15 and puts r6 in a
   slot, addl $5, %eax and movl %eax, -52(%rbp), with the warning;
   its leaf frame has no sub, so the epilogue pops after no leaq; and
   t takes rbx again, imull $2, -44(%rbp), %ebx.

   On aarch64, whose char is unsigned and whose register variables are
   x19 to x28, compound_in_place() is add w19, w19, #5 and add w19,
   w19, #1.  narrow_store() extends the unwritten c, uxtb w19, w19,
   and stores g's result by uxtb w19, w0; narrow_step() is add w19,
   w19, #1 and uxtb w19, w19 after it; and narrow_unwritten() the one
   uxtb w19, w19 before mov w0, w19.  held_step() copies, mov w0, w19
   and mov w1, w19, before add w19, w19, #1.  held_and(), held_or()
   and held_conditional() store the variable's register before the
   first branch, str x19, [x29, #32], copy the read before the step,
   mov w0, w19, and reload the saved read where the paths meet, ldr
   x1, [x29, #32].  The argument registers are eight: stacked() moves
   s from its register, mov w19, w6, among the stores, and loads only
   k from the caller's area, ldr w20, [x29, #64], first, before the
   stores of the other parameters.  mixed() is str w0, [x29, #32], mov
   w19, w1, str w2, [x29, #36] and mov w20, w3, in that order.  The
   epilogue loads the saved registers from the save area off x29,
   ldr x19, [x29, #16], with no counterpart of x86-64's leaq, whatever
   the frame: epilogue() is ldr x19, [x29, #16], ldp x29, x30, [sp]
   and add sp, sp, #48.  sixth() has ten registers and no slot, the
   sixth variable in x24, add w24, w24, #5, and t takes x19 again, mul
   w19, w19, w0. */
int g(int);

int
compound_in_place(int a)
{
        register int r = a;

        r += 5;
        ++r;

        return r;
}

int
narrow_store(int a)
{
        register char c;

        c = g(a);

        return c;
}

int
narrow_step(char x)
{
        register char c = x;

        c++;

        return c;
}

int
narrow_unwritten(void)
{
        register char c;

        return c;
}

int
held_step(int a)
{
        register int i = a;

        return i + i++;
}

int
held_and(int a, int b)
{
        register int i = a;

        return i + (b && i++);
}

int
held_or(int a, int b)
{
        register int i = a;

        return i + (b || i++);
}

int
held_conditional(int a, int b)
{
        register int i = a;

        return i + (b ? i++ : 0);
}

int
stacked(int a,
        int b,
        int c,
        int d,
        int e,
        int f,
        register int s,
        int h,
        register int k)
{
        return a + b + c + d + e + f + s + h + k;
}

int
mixed(int a, register int b, int c, register int d)
{
        return a + b + c + d;
}

int
epilogue(int a)
{
        register int r = a;
        int m[4];

        m[0] = r;

        return g(m[0]) + r;
}

int
sixth(int a)
{
        int s = 0;
        {
                register int r1 = a, r2 = a + 1, r3 = a + 2, r4 = a + 3;
                register int r5 = a + 4, r6 = a + 5;

                s = r1 + r2 + r3 + r4 + r5 + r6;
        }
        {
                register int t = a * 2;

                s = s + t;
        }

        return s;
}
