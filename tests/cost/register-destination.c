/* backend.md chapter 4 and aarch64.md chapter 4, the assignment to a
   register variable: the variable's register is the destination of
   the value computed for it, where the last instruction can be the
   one that writes it, and no mov follows.  Each function's variable
   is r, or p for a pointer, in the first register variable's register
   but in register_leaf(), where the register parameter s takes it and
   r the second.

   function_defined() and function_declared() take a function's
   address, and address_far() an element of an array beyond the reach
   of an add's 12 bits.  signedness_load(), signedness_char() and
   signedness_byte() initialize the variable from an object under a
   conversion that changes the signedness alone, register unsigned r =
   i, register char r = uc and register unsigned char r = sc;
   signedness_sum(), signedness_unsigned_sum() and signedness_compare()
   assign it a value so converted, r = a + b on two ints and on two
   unsigneds and r = a < b; beside them signedness_narrowed() and
   signedness_widened() convert by a narrowing and a widening, r =
   (int) l and register unsigned r = c, which keep the mov.

   decay_sum(), decay_row() and decay_twice() write a sum whose left
   operand is an array's decay, p = characters + n, p = rows + n of 12
   bytes and p = characters + i + i, and decay_declared() one of an
   object only declared, &declared + n; decay_named() a sum whose right
   operand names the variable, p = characters + *p, which keeps the
   mov.  own_value() writes an address that is a pointer's own value, p
   = &*q and p = &q[0], and element_row() the address of an element of
   12 bytes, p = &rows[i].  not_int() and not_long() write r = !x, and
   not_char() the same into a char, a narrowing.  step_value(),
   step_pointer() and step_register() write the value of a step, r =
   x++ and r = x-- on a local, r = (*p)++ and r = s++ on a register
   variable; beside them step_indexed(), r = a[r]++, whose address
   reads the variable, and step_widened(), a step widened to
   long.  mixed_sum() and promoted_sum() write u = i + ub and u = c + b
   into an unsigned, and narrowed_sum() u = (int) l + b beside them.

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
   addq $4, %rbx after the initializer's load, imulq $4, -24(%rbp),
   %rax, n the multiply's source, and subq %rax, %rbx, and
   movq -16(%rbp), %rbx; subq $4, %rbx; in negated() and
   complemented(), the operand loaded into the register
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
   both operands before it writes, and after the multiply with n its
   source, imulq $4, -24(%rbp), %rax and subq %rax, %rbx, and
   imulq $12, -24(%rbp), %rax and addq %rax, %rbx or subq %rax, %rbx,
   and no mov into the register.  In function_defined() and
   function_declared() the address into the register, leaq
   defined_function(%rip), %rbx and movq
   declared_function@GOTPCREL(%rip), %rbx, and leaq -2008(%rbp), %rbx
   in address_far(); the load at the conversion's extension into the
   register, movl -12(%rbp), %ebx in signedness_load(), movsbl
   -9(%rbp), %ebx in signedness_char() and movzbl -9(%rbp), %ebx in
   signedness_byte(); the operation on the register, movl -12(%rbp),
   %ebx; addl -16(%rbp), %ebx, in signedness_sum() and
   signedness_unsigned_sum(), and setl %bl; movzbl %bl, %ebx after the
   cmp in signedness_compare(); and the mov kept, movl %eax, %ebx,
   after movq -16(%rbp), %rax in signedness_narrowed() and after movsbl
   -9(%rbp), %eax in signedness_widened().  In decay_sum() and
   decay_twice() the decay's leaq into the register, leaq
   characters(%rip), %rbx, then addq -16(%rbp), %rbx once and twice; in
   decay_row() leaq rows(%rip), %rbx, imulq $12, -16(%rbp), %rax and
   addq %rax, %rbx; in decay_declared() the GOT's load into rax and
   leaq (%rax,%rcx,4), %rbx; in decay_named() the sum computed in rax
   after movsbq (%rbx), %rcx and moved, movq %rax, %rbx; in own_value()
   movq -16(%rbp), %rbx twice; in element_row() the sum of decay_row();
   sete %bl; movzbl %bl, %ebx after the cmp in not_int() and
   not_long(), and in not_char() sete %al; movzbl %al, %eax and the
   extension, movsbl %al, %ebx; in step_value() the load into the
   register, movl -12(%rbp), %ebx, then movl %ebx, %eax, addl $1, %eax
   or subl $1, %eax, and the store, movl %eax, -12(%rbp), and the same
   through p's load in step_pointer(), movl (%rax), %ebx; in
   step_register() the copy into r's register, movl %ebx, %r12d, before
   addl $1, %ebx; in step_indexed() the load into rcx and the mov kept,
   movq %rdx, %rbx, and in step_widened() movslq %ecx, %rcx; movq %rcx,
   %rbx; in mixed_sum() and promoted_sum() movl -12(%rbp), %ebx and
   movsbl -9(%rbp), %ebx, each then addl -16(%rbp), %ebx; and in
   narrowed_sum() the sum in eax and movl %eax, %ebx.  return r is movl
   %ebx, %eax, and return p movq %rbx, %rax.

   On aarch64 the listing must show the last instruction of each
   right operand writing the register, no condition on the operands
   and nothing refused: ldr w19, [x29, #24] in load(); add w19, w0,
   #5, add w19, w0, w1 and add w20, w0, w19 for the leaves, the char
   loaded by ldrb w1, [x29, #28]; add w19, w0, w1 after add w1, w1, w2
   in nested(); sub w19, w0, w19 in named(), after the initializer's
   ldr w19, [x29, #28]; add w19, w19, w0 in itself(); add w0, w19, #1
   and add w19, w0, w1 in itself_left(); sdiv w19, w0, w1 in
   quotient(); mul w19, w0, w1 in initializer(); the address of a
   global in two instructions written into the register, adrp x19,
   defined and add x19, x19, :lo12:defined, and the GOT's adrp x19,
   :got:declared and ldr x19, [x19, :got_lo12:declared], and no mov;
   add x19, x29, #24 and add x19, x0, w1, sxtw #2 for the
   local and the element; add x19, x0, x1, lsl #2, add x19, x19, #4,
   sub x19, x19, x0, lsl #2 and sub x19, x0, #4 for the pointers; neg
   w19, w0, mvn w19, w0 and neg w19, w19; cset w19, lt and cset w19, eq
   after the cmp; add w19, w0, w1 after the element's ldr in
   element_leaf() and pointer_element_leaf(); add w19, w0, w19 in
   named_right(), and add x19, x0, x1 after ldrb w1, [x19, #1] in
   own_element(); for p += n and p -= n, add x19, x19, x0 and sub x19,
   x19, x0, add x19, x19, x0, lsl #2 and sub x19, x19, x0, lsl #2, and
   add x19, x19, x0 and sub x19, x19, x0 after mov x1, #12 and mul x0,
   x0, x1; adrp x19, defined_function and add x19, x19,
   :lo12:defined_function in function_defined(), adrp x19,
   :got:declared_function and ldr x19, [x19,
   :got_lo12:declared_function] in function_declared(), and add x19,
   x29, #4096; add x19, x19, #1928 in address_far(); ldr w19, [x29,
   #24] in signedness_load(), ldrb w19, [x29, #24] in signedness_char()
   and signedness_byte(); add w19, w0, w1 in signedness_sum() and
   signedness_unsigned_sum(), and cset w19, lt in signedness_compare();
   and the mov kept, mov w19, w0, after ldr x0, [x29, #24] in
   signedness_narrowed() and after ldrb w0, [x29, #24] in
   signedness_widened(); add x19, x0, x1, add x19, x0, x1, lsl #2 and
   add x19, x0, x1 after the mul by 12 in the decay functions, add x19,
   x0, x1 in decay_named() after ldrb w1, [x19], ldr x19, [x29, #24]
   twice in own_value(); cset w19, eq in not_int() and not_long(), and
   uxtb w19, w0 after the cset in not_char(); the load into the
   register, ldr w19, [x29, #24], then add w0, w19, #1 or sub w0, w19,
   #1 and the store, str w0, [x29, #24], in step_value(), ldr w19, [x0]
   in step_pointer(), mov w20, w19 before add w19, w19, #1 in
   step_register(), the load into x1 and mov x19, x1 in step_indexed(),
   and sxtw x19, w0 in step_widened(); add w19, w0, w1 in mixed_sum(),
   promoted_sum() and narrowed_sum().  return r is mov w0, w19, and
   return p mov x0, x19. */
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

int
defined_function(void)
{
        return 0;
}

extern int declared_function(void);

int (*function_defined(void))(void)
{
        register int (*p)(void);

        p = defined_function;
        return p;
}

int (*function_declared(void))(void)
{
        register int (*p)(void);

        p = declared_function;
        return p;
}

long
address_far(void)
{
        int big[2000];
        register int *p;

        p = &big[1500];
        return p - big;
}

unsigned
signedness_load(int i)
{
        register unsigned r = i;

        return r;
}

int
signedness_char(unsigned char uc)
{
        register char r = uc;

        return r;
}

int
signedness_byte(signed char sc)
{
        register unsigned char r = sc;

        return r;
}

unsigned
signedness_sum(int a, int b)
{
        register unsigned r;

        r = a + b;
        return r;
}

int
signedness_unsigned_sum(unsigned a, unsigned b)
{
        register int r;

        r = a + b;
        return r;
}

unsigned
signedness_compare(int a, int b)
{
        register unsigned r;

        r = a < b;
        return r;
}

unsigned
signedness_narrowed(long l)
{
        register unsigned r;

        r = (int) l;
        return r;
}

unsigned
signedness_widened(char c)
{
        register unsigned r = c;

        return r;
}

char characters[16];
int rows[4][3];

char *
decay_sum(long n)
{
        register char *p;

        p = characters + n;
        return p;
}

char *
decay_declared(long n)
{
        register int *p;

        p = &declared + n;
        return (char *) p;
}

int (*decay_row(long n))[3]
{
        register int (*p)[3];

        p = rows + n;
        return p;
}

char *
decay_twice(long i)
{
        register char *p;

        p = characters + i + i;
        return p;
}

char *
decay_named(char *q)
{
        register char *p = q;

        p = characters + *p;
        return p;
}

int *
own_value(int *q)
{
        register int *p;

        p = &*q;
        p = &q[0];
        return p;
}

int (*element_row(long i))[3]
{
        register int (*p)[3];

        p = &rows[i];
        return p;
}

int
not_int(int x)
{
        register int r;

        r = !x;
        return r;
}

long
not_long(long x)
{
        register long r;

        r = !x;
        return r;
}

int
not_char(int x)
{
        register char r;

        r = !x;
        return r;
}

int
step_value(int x)
{
        register int r;

        r = x++;
        r = r + x;
        r = x--;
        return r + x;
}

int
step_pointer(int *p)
{
        register int r;

        r = (*p)++;
        return r;
}

int
step_register(int a)
{
        register int s = a;
        register int r;

        r = s++;
        return r + s;
}

long
step_indexed(long *a, long i)
{
        register long r = i;

        r = a[r]++;
        return r;
}

long
step_widened(int x)
{
        register long r;

        r = x++;
        return r + x;
}

unsigned
mixed_sum(int i, unsigned ub)
{
        register unsigned u;

        u = i + ub;
        return u;
}

unsigned
promoted_sum(char c, int b)
{
        register unsigned u;

        u = c + b;
        return u;
}

unsigned
narrowed_sum(long l, int b)
{
        register unsigned u;

        u = (int) l + b;
        return u;
}
