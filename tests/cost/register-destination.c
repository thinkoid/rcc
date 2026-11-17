/* backend.md chapter 4 and aarch64.md chapter 4, the assignment to a
   register variable: the variable's register is the destination of
   the value computed for it, where the last instruction can be the
   one that writes it, and no mov follows.  Each function's variable
   is r, in the first register variable's register but in
   register_leaf(), where the register parameter s takes it and r the
   second.

   On x86-64 the listing must show: in load(), the load of a into the
   register, movl -12(%rbp), %ebx; in constant_leaf(), named_leaf(),
   register_leaf(), pointer_leaf() and promoted_leaf(), the operation
   on the register, a loaded into it and the instruction written on it
   with the right operand, the leaf, addl $5, %ebx, addl -16(%rbp),
   %ebx, addl %ebx, %r12d, addl (%rax), %ebx after p's load, and
   addl %eax, %ebx after the char's movsbl -13(%rbp), %eax; in nested()
   and named(), the two shapes the form refuses, a right operand that
   is no leaf and a right operand that names r: the sum computed in
   eax and moved, movl %eax, %ebx; in named(), the initializer the
   load into the register, movl -16(%rbp), %ebx; in itself(), L the
   variable itself and nothing written for it, addl -16(%rbp), %ebx
   after the initializer's load; in itself_left(), addl $1, %ebx and
   addl -16(%rbp), %ebx; in quotient(), the division as before,
   cltd and idivl -16(%rbp), and the quotient moved, movl %eax, %ebx;
   in initializer(), the initializer's operation on the register,
   movl -12(%rbp), %ebx and imull -16(%rbp), %ebx.  return r is
   movl %ebx, %eax.

   On aarch64 the listing must show the last instruction of each
   right operand writing the register, no condition on the operands
   and nothing refused: ldr w19, [x29, #24] in load(); add w19, w0,
   #5, add w19, w0, w1 and add w20, w0, w19 for the leaves, the char
   loaded by ldrb w1, [x29, #28]; add w19, w0, w1 after add w1, w1, w2
   in nested(); sub w19, w0, w19 in named(), after the initializer's
   ldr w19, [x29, #28]; add w19, w19, w0 in itself(); add w0, w19, #1
   and add w19, w0, w1 in itself_left(); sdiv w19, w0, w1 in
   quotient(); mul w19, w0, w1 in initializer().  return r is mov w0,
   w19. */
int
load(int a)
{
        register int r;

        r = a;
        return r;
}

int
constant_leaf(int a)
{
        register int r;

        r = a + 5;
        return r;
}

int
named_leaf(int a, int b)
{
        register int r;

        r = a + b;
        return r;
}

int
register_leaf(register int s, int a)
{
        register int r;

        r = a + s;
        return r;
}

int
pointer_leaf(int a, int *p)
{
        register int r;

        r = a + *p;
        return r;
}

int
promoted_leaf(int a, char c)
{
        register int r;

        r = a + c;
        return r;
}

int
nested(int a, int b, int c)
{
        register int r;

        r = a + (b + c);
        return r;
}

int
named(int a, int b)
{
        register int r = b;

        r = a - r;
        return r;
}

int
itself(int a, int b)
{
        register int r = a;

        r = r + b;
        return r;
}

int
itself_left(int a, int b)
{
        register int r = a;

        r = (r + 1) + b;
        return r;
}

int
quotient(int a, int b)
{
        register int r;

        r = a / b;
        return r;
}

int
initializer(int a, int b)
{
        register int r = a * b;

        return r;
}
