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

   logical_and() and logical_or() write r = a && b and r = a || b, and
   logical_itself() r = r && b, whose condition reads the variable;
   chosen() writes r = c ? x : y, and chosen_reading() r = c ? r + 1 :
   0, whose first arm reads it; chosen_called() and chosen_stepped()
   r = c ? f(x) : 0 and r = c ? f(x) : p[i]++, whose first arm's call
   writes no variable, and chosen_divided() r = c ? f(x) : a / b, whose
   second arm's division writes none either on x86-64; chosen_widened()
   and chosen_constant() a conditional widened to a long, r = c ? x : y
   and r = c ? x : -1, and chosen_promoted() r = c ? a : b on two
   chars, each arm a load under a promotion;
   chosen_calls() r = c ? f(x) : f(x + 1), whose arms are two calls;
   and chosen_own_first(), chosen_own_second() and chosen_own_pointer()
   r = c ? r : x, r = c ? x : r and q = c ? p : q on a register pointer
   q, an arm that is the variable's own read, beside itself_assigned(),
   r = r.

   char_operand() and char_operand_left() write r = c + 5 and r = 5 +
   c on a register char c, whose read is valid to the int's width,
   int_widened() r = i + 5L on a register int i, whose read is not
   valid to the long's, and register_product() r = x * 4 on a register
   variable.  prefix_step() and prefix_pointer() write the value of a
   prefix step on an object in memory, r = ++x, r = --x and q = ++p,
   and prefix_char() r = ++c on a char, whose sum is no char.
   row_element() and row_decay() write p = &rows[i][0] and p = rows[i]
   on rows of 12 bytes.  widened_sum() and widened_register() write a
   value extended into a register long, r = a + b from two ints and
   r = i from a register int; widened_product() r = a * b into a
   register unsigned long from two unsigneds, whose 32-bit write is the
   widening; and signedness_left() v = 5 + (int) u into a register
   unsigned from an unsigned in memory, whose (int) u, a load under a
   conversion of the signedness alone, is the left operand.

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
   addl -16(%rbp), %ebx; in itself_assigned() nothing written for r = r
   after the initializer's load; in quotient(), the division as before,
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
   movq %rdx, %rbx, and in step_widened() movslq %ecx, %rbx; in
   mixed_sum() and promoted_sum() movl -12(%rbp), %ebx and
   movsbl -9(%rbp), %ebx, each then addl -16(%rbp), %ebx; and in
   narrowed_sum() the sum in eax and movl %eax, %ebx; in logical_and(),
   logical_or() and logical_itself() movl $1, %ebx and movl $0, %ebx on
   the two paths, logical_itself()'s cmpl $0, %ebx before them; in
   chosen() movl -16(%rbp), %ebx and movl -20(%rbp), %ebx on the two
   paths, and in chosen_reading() addl $1, %ebx and movl $0, %ebx, each
   with no mov into ebx after the join; in chosen_called() and
   chosen_stepped() movl %eax, %ebx after the call on its own path, and
   on the other movl $0, %ebx and the step's load movl (%rax,%rcx,4),
   %ebx; in chosen_widened() movslq -16(%rbp), %rbx and movslq
   -20(%rbp), %rbx on the two paths, and in chosen_constant() movslq
   -16(%rbp), %rbx and movq $-1, %rbx, with no instruction after the
   join; in chosen_divided() the call's result and the quotient joined
   in eax and movl %eax, %ebx after the join, and in chosen_promoted()
   movsbq -13(%rbp), %rbx and movsbq -14(%rbp), %rbx on the two paths;
   in chosen_calls() the calls' results joined in eax and movl %eax,
   %ebx after the join, for neither arm's row writes the variable; in
   chosen_own_first() nothing written on the first path and movl
   -16(%rbp), %ebx on the second, in chosen_own_second() the load on
   the first and nothing on the second, and in chosen_own_pointer() the
   same with movq -24(%rbp), %rbx.  In
   char_operand() and char_operand_left() one listing, movl %ebx,
   %r12d; addl $5, %r12d; in int_widened() the extension into the
   register, movslq %ebx, %r12, and addq $5, %r12; in register_product()
   imull $4, %ebx, %r12d; in prefix_step() the load into the register,
   the step on it and the store from it, movl -12(%rbp), %ebx; addl $1,
   %ebx; movl %ebx, -12(%rbp), and the same with subl $1, %ebx, and in
   prefix_pointer() movq -16(%rbp), %rbx; addq $4, %rbx; movq %rbx,
   -16(%rbp); in prefix_char() the step in eax, the store from al and
   the extension, movsbl %al, %ebx; in row_element() and row_decay()
   leaq rows(%rip), %rbx, imulq $12, -16(%rbp), %rax and addq %rax,
   %rbx; in widened_sum() movslq %eax, %rbx after the add, and in
   widened_register() movslq %ebx, %r12; in widened_product() movl
   -12(%rbp), %ebx; imull -16(%rbp), %ebx, and in signedness_left()
   movl -12(%rbp), %ebx; addl $5, %ebx, each with no mov into the
   register.  return r is movl %ebx, %eax, and return p movq %rbx,
   %rax.

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
   promoted_sum() and narrowed_sum(); mov w19, #1 and mov w19, #0 on the
   two paths in logical_and(), logical_or() and logical_itself(), after
   cbz w19 in logical_itself(); ldr w19, [x29, #28] and ldr w19, [x29,
   #32] in chosen(), and add w19, w19, #1 and mov w19, #0 in
   chosen_reading(), with no mov into w19 after the join; in
   chosen_called(), chosen_stepped() and chosen_divided() mov w19, w0
   after the call on its own path, and on the other mov w19, #0, the
   step's load ldr w19, [x0, w1, sxtw #2] and sdiv w19, w0, w1; in
   chosen_widened() ldrsw x19, [x29, #28] and ldrsw x19, [x29, #32] on
   the two paths, and in chosen_constant() ldrsw x19, [x29, #28] and
   mov x19, #-1, with no instruction after the join; in
   chosen_promoted() the two ldrb joined in w0 and sxtw x19, w0 after
   it, and in chosen_calls() the two calls' results joined in w0 and
   mov w19, w0 after it; in chosen_own_first() nothing written on the
   first path and ldr w19, [x29, #28] on the second, in
   chosen_own_second() and chosen_own_pointer() the load into the
   register on the first path and nothing on the second, and in
   itself_assigned() nothing after ldr w19, [x29, #24]; add w20, w19,
   #5 in char_operand() and char_operand_left(); sxtw x0, w19 and add
   x20, x0, #5 in
   int_widened(); mul w20, w19, w0 after mov w0, #4 in
   register_product(); in prefix_step() the step written
   into the register and the store from it, add w19, w0, #1; str w19,
   [x29, #24], and sub w19, w0, #1, and in prefix_pointer() add x19,
   x0, #4; str x19, [x29, #24]; in prefix_char() add w0, w0, #1, strb
   w0, [x29, #24] and uxtb w19, w0; add x19, x0, x1 after the mul by 12
   in row_element() and row_decay(); sxtw x19, w0 in widened_sum() and
   sxtw x20, w19 in widened_register(); mul w19, w0, w1 in
   widened_product() and add w19, w0, #5 after ldr w0, [x29, #24] in
   signedness_left().  return r is mov w0, w19, and return p mov x0,
   x19. */
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
itself_assigned(int a)
{
        register int r = a;

        r = r;
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

int f(int);

int
logical_and(int a, int b)
{
        register int r;

        r = a && b;
        return r;
}

int
logical_or(int a, int b)
{
        register int r;

        r = a || b;
        return r;
}

int
logical_itself(int a, int b)
{
        register int r = a;

        r = r && b;
        return r;
}

int
chosen(int c, int x, int y)
{
        register int r;

        r = c ? x : y;
        return r;
}

int
chosen_reading(int c, int x)
{
        register int r = x;

        r = c ? r + 1 : 0;
        return r;
}

int
chosen_called(int c, int x)
{
        register int r;

        r = c ? f(x) : 0;
        return r;
}

long
chosen_widened(int c, int x, int y)
{
        register long r;

        r = c ? x : y;
        return r;
}

int
chosen_stepped(int c, int x, int *p, int i)
{
        register int r;

        r = c ? f(x) : p[i]++;
        return r;
}

int
chosen_divided(int c, int x, int a, int b)
{
        register int r;

        r = c ? f(x) : a / b;
        return r;
}

long
chosen_constant(int c, int x)
{
        register long r;

        r = c ? x : -1;
        return r;
}

long
chosen_promoted(int c, char a, char b)
{
        register long r;

        r = c ? a : b;
        return r;
}

int
chosen_calls(int c, int x)
{
        register int r;

        r = c ? f(x) : f(x + 1);
        return r;
}

int
chosen_own_first(int c, int x)
{
        register int r = x;

        r = c ? r : x;
        return r;
}

int
chosen_own_second(int c, int x)
{
        register int r = x;

        r = c ? x : r;
        return r;
}

int *
chosen_own_pointer(int c, int *p)
{
        register int *q = p;

        q = c ? p : q;
        return q;
}

int
char_operand(char x)
{
        register char c = x;
        register int r;

        r = c + 5;
        return r;
}

int
char_operand_left(char x)
{
        register char c = x;
        register int r;

        r = 5 + c;
        return r;
}

long
int_widened(int a)
{
        register int i = a;
        register long r;

        r = i + 5L;
        return r;
}

int
register_product(int a)
{
        register int x = a;
        register int r;

        r = x * 4;
        return r + x;
}

int
prefix_step(int x)
{
        register int r;

        r = ++x;
        r = r + x;
        r = --x;
        return r + x;
}

int *
prefix_pointer(int *p)
{
        register int *q;

        q = ++p;
        return q;
}

int
prefix_char(char c)
{
        register int r;

        r = ++c;
        return r;
}

int *
row_element(long i)
{
        register int *p;

        p = &rows[i][0];
        return p;
}

int *
row_decay(long i)
{
        register int *p;

        p = rows[i];
        return p;
}

long
widened_sum(int a, int b)
{
        register long r;

        r = a + b;
        return r;
}

long
widened_register(int a)
{
        register int i = a;
        register long r;

        r = i;
        return r + i;
}

unsigned long
widened_product(unsigned a, unsigned b)
{
        register unsigned long r;

        r = a * b;
        return r;
}

unsigned
signedness_left(unsigned u)
{
        register unsigned v;

        v = 5 + (int) u;
        return v;
}
