/* backend.md chapter 4 and aarch64.md chapter 4, the assignment to a
   register variable: the variable's register is the destination of
   the value computed for it, where the last instruction can be the
   one that writes it, and no mov follows.  Each function's variable
   is r, or p for a pointer, in the first register variable's register
   but in register_leaf(), where the register parameter s takes it and
   r the second.

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
   movl -12(%rbp), %ebx and imull -16(%rbp), %ebx; in
   address_global(), address_declared(), address_local() and
   address_element(), the address written into the register, leaq
   defined(%rip), %rbx, the GOT's load movq declared@GOTPCREL(%rip),
   %rbx, leaq -12(%rbp), %rbx and leaq -28(%rbp,%rax,4), %rbx; in
   pointer_sum(), leaq (%rax,%rcx,4), %rbx; in pointer_step(),
   pointer_back() and pointer_before(), the operation on the register,
   addq $4, %rbx after the initializer's load, imulq $4, %rax, %rax and
   subq %rax, %rbx, and movq -16(%rbp), %rbx; subq $4, %rbx; in
   negated() and complemented(), the operand loaded into the register
   and negl %ebx or notl %ebx on it, and in negated_itself() the negl
   alone; in compared(), compared_itself() and compared_constant(), the
   cmp, then setl %bl and movzbl %bl, %ebx, the cmp reading r in its
   register in compared_itself(), cmpl -16(%rbp), %ebx, and sete %bl
   after cmpl $5, -12(%rbp); in element_leaf() and
   pointer_element_leaf(), an element by a constant index as the leaf,
   addl table+12(%rip), %ebx, and addl 12(%rax), %ebx after p's load;
   in named_right() and own_element(), a right operand that names the
   variable, as itself and as the base of the element's address, p[1],
   refused: the sum computed in eax and in rax and moved, movl %eax,
   %ebx and movq %rax, %rbx; in step_char(), back_char(), step_int(),
   back_int(), step_row() and back_row(), p += n and p -= n at sizes
   1, 4 and 12, the step on the register after n's load, addq %rax,
   %rbx and subq %rax, %rbx, leaq (%rbx,%rax,4), %rbx, which reads
   both operands before it writes, imulq $4, %rax, %rax and subq %rax,
   %rbx, and imulq $12, %rax, %rax and addq %rax, %rbx or subq %rax,
   %rbx, and no mov into the register.  return r is movl %ebx, %eax,
   and return p movq %rbx, %rax.

   On aarch64 the listing must show the last instruction of each
   right operand writing the register, no condition on the operands
   and nothing refused: ldr w19, [x29, #24] in load(); add w19, w0,
   #5, add w19, w0, w1 and add w20, w0, w19 for the leaves, the char
   loaded by ldrb w1, [x29, #28]; add w19, w0, w1 after add w1, w1, w2
   in nested(); sub w19, w0, w19 in named(), after the initializer's
   ldr w19, [x29, #28]; add w19, w19, w0 in itself(); add w0, w19, #1
   and add w19, w0, w1 in itself_left(); sdiv w19, w0, w1 in
   quotient(); mul w19, w0, w1 in initializer(); the address of a
   global in two instructions, adrp x0, defined and add x0, x0,
   :lo12:defined, and the GOT's adrp and ldr, each into x0 and moved,
   mov x19, x0; add x19, x29, #24 and add x19, x0, w1, sxtw #2 for the
   local and the element; add x19, x0, x1, lsl #2, add x19, x19, #4,
   sub x19, x19, x0, lsl #2 and sub x19, x0, #4 for the pointers; neg
   w19, w0, mvn w19, w0 and neg w19, w19; cset w19, lt and cset w19, eq
   after the cmp; add w19, w0, w1 after the element's ldr in
   element_leaf() and pointer_element_leaf(); add w19, w0, w19 in
   named_right(), and add x19, x0, x1 after ldrb w1, [x19, #1] in
   own_element(); for p += n and p -= n, add x19, x19, x0 and sub x19,
   x19, x0, add x19, x19, x0, lsl #2 and sub x19, x19, x0, lsl #2, and
   add x19, x19, x0 and sub x19, x19, x0 after mov x1, #12 and mul x0,
   x0, x1.  return r is mov w0, w19, and return p mov x0, x19. */
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

int defined;
extern int declared;
int table[8];

int *
address_global(void)
{
        register int *p;

        p = &defined;
        return p;
}

int *
address_declared(void)
{
        register int *p;

        p = &declared;
        return p;
}

int
address_local(int x)
{
        register int *p;

        p = &x;
        return *p;
}

int
address_element(int i)
{
        int a[4];
        register int *p;

        p = &a[i];
        return *p;
}

int *
pointer_sum(int *q, long n)
{
        register int *p;

        p = q + n;
        return p;
}

int *
pointer_step(int *q)
{
        register int *p = q;

        p = p + 1;
        return p;
}

int *
pointer_back(int *q, long n)
{
        register int *p = q;

        p = p - n;
        return p;
}

int *
pointer_before(int *q)
{
        register int *p;

        p = q - 1;
        return p;
}

int
negated(int x)
{
        register int r;

        r = -x;
        return r;
}

int
complemented(int x)
{
        register int r;

        r = ~x;
        return r;
}

int
negated_itself(int x)
{
        register int r = x;

        r = -r;
        return r;
}

int
compared(int x, int y)
{
        register int r;

        r = x < y;
        return r;
}

int
compared_itself(int x, int y)
{
        register int r = x;

        r = r < y;
        return r;
}

int
compared_constant(int x)
{
        register int r;

        r = x == 5;
        return r;
}

int
element_leaf(int x)
{
        register int r;

        r = x + table[3];
        return r;
}

int
pointer_element_leaf(int x, int *p)
{
        register int r;

        r = x + p[3];
        return r;
}

int
named_right(int x, int y)
{
        register int r = y;

        r = x + r;
        return r;
}

char *
own_element(char *q)
{
        register char *p = q;

        p = q + p[1];
        return p;
}

char *
step_char(char *q, long n)
{
        register char *p = q;

        p += n;
        return p;
}

char *
back_char(char *q, long n)
{
        register char *p = q;

        p -= n;
        return p;
}

int *
step_int(int *q, long n)
{
        register int *p = q;

        p += n;
        return p;
}

int *
back_int(int *q, long n)
{
        register int *p = q;

        p -= n;
        return p;
}

int (*step_row(int (*q)[3], long n))[3]
{
        register int (*p)[3] = q;

        p += n;
        return p;
}

int (*back_row(int (*q)[3], long n))[3]
{
        register int (*p)[3] = q;

        p -= n;
        return p;
}
