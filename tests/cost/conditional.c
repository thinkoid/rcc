/* backend.md 7.2 and aarch64.md 7.2, e ? a : b for its value: jump
   code, never a conditional move.  backend.md 6.1, the join of the two
   arms is the register the conditional's consumer names, where it
   names one, and otherwise the first arm's, and each arm writes it
   where its last instruction can: stepped() returns a postfix step's
   old value or b; stored() stores a remainder or d, a consumer that
   names no register; summed() adds e to the joined value, the
   conditional the left operand of a returned sum; widened() returns
   an int remainder or c as a long, the conversion handed to each arm,
   and widened_constants() 4 or -5 as a long; widened_quotients()
   returns one of two quotients as a long, each of whose arms would
   need an extension of its own, which keeps the join and the one
   extension after it; widened_stored() stores a remainder or c into a
   long, a consumer that names no register; summed_arm() returns 0 or
   x + (d ? 1 : 2), whose x is spilled before the inner branch;
   widened_step() returns a postfix step's old value or b as a long,
   widened_chars() the same on chars, whose arms each need an extension
   of their own, and narrowed_stored() b or the value of *p = b as a
   char.  Beside them the edge: widened_called() returns as a long, and
   joined_called() as an int, the value of an assignment through one of
   two calls' results, whose place holds the return's register.
   widened_chars_stored() stores b or c, two signed chars, into a long,
   each arm's load the extension, and widened_sums_stored() a sum or a
   difference, neither of which widens free.

   On x86-64 the listing must show in conditional() a compared in its
   slot, cmpl $0, -4(%rbp), a je to the arm that loads c, the arm that
   loads b into the same eax and jumps past it, the two joining at the
   jmp to the epilogue label, and no cmov.  In stepped() the element's
   address in rcx and rdx, its load into esi and the old value copied
   into eax, movl %esi, %eax, before the step and the store, and the
   second arm's load into eax, movl -8(%rbp), %eax, with no move after
   the join.  In stored() the arms join in edx, the remainder's
   register, the second arm's load written there, movl -16(%rbp),
   %edx, and the store from it, movl %edx, g(%rip).  In summed() the
   remainder moved into eax, movl %edx, %eax, before its jump, the
   second arm's load into eax and addl -20(%rbp), %eax after the join.
   In widened() the remainder extended into rax, movslq %edx, %rax,
   and the second arm's load extended there, movslq -12(%rbp), %rax,
   with nothing after the join; in widened_constants() movq $4, %rax
   and movq $-5, %rax on the two paths and nothing after the join; in
   widened_quotients() the two quotients joined in eax and movslq %eax,
   %rax after the join; in widened_stored() the remainder extended in
   rdx, movslq %edx, %rdx, the second arm's load extended there,
   movslq -12(%rbp), %rdx, and the store movq %rdx, h(%rip) after the
   join.  In summed_arm() the first arm's movl $0, %eax, and on the
   second x reloaded into rax and addl %ecx, %eax.  In widened_step()
   the old value copied into eax, movl %edx, %eax, and extended on its
   own path, movslq %eax, %rax, the second arm's load extended, movslq
   -20(%rbp), %rax, and nothing after the join.  In widened_chars() the
   old value copied into eax and extended on its own path, movslq
   %eax, %rax, the second arm's one load movsbq -17(%rbp), %rax, and
   nothing after the join; in narrowed_stored() the two
   movsbl -17(%rbp), %eax and the store movb %al, (%rcx); in
   widened_called() and joined_called() the store through each call's
   result, movl %ecx, (%rax), and movl %ecx, %eax on each path, then
   movslq %eax, %rax after the join in widened_called().  In
   widened_chars_stored() movsbq -5(%rbp), %rax and movsbq -6(%rbp),
   %rax on the two paths and the store movq %rax, h(%rip) after the
   join; in widened_sums_stored() the sum and the difference joined in
   eax, movslq %eax, %rax after the join and the store.

   On aarch64 the listing must show in conditional() a tested with cbz
   w0 to the arm that loads c, the arm that loads b into the same w0
   and branches past it, the two joining at the b to the epilogue
   label, and no csel.  In stepped() the element's address in x1 and
   w2 and the old value loaded into w0, the return's x0, ldr w0, [x1,
   w2, sxtw #2], before the step and the store, and the second arm's
   ldr w0, [x29, #20], with no move after the join.  In stored() and
   summed() msub w0, w2, w1, w0 and the second arm's load into w0,
   then the store and add w0, w0, w1.  Under the cast of a return the
   arms are extended into x0 where one of them is free: in widened()
   the msub and sxtw x0, w0 on its path and ldrsw x0, [x29, #24] on
   the other, in widened_constants() mov x0, #4 and mov x0, #-5, and
   in widened_step() the old value loaded into w0 and sxtw x0, w0 on
   its path, and ldrsw x0, [x29, #32] on the other, each with nothing
   after the join; in widened_quotients() the two sdiv w0, w0, w1 and
   in widened_chars() the old value loaded into w0 and extended on its
   own path, sxtw x0, w0, and the second arm's ldrb w0, [x29, #32],
   valid to 64 bits, with nothing after the join.  Under a consumer
   that names no register the same: in widened_stored() the msub and
   sxtw x0, w0 on its path, ldrsw x0, [x29, #24] on the other and str
   x0, [x16, :lo12:h] after the join, in widened_chars_stored() ldrsb
   x0, [x29, #20] and ldrsb x0, [x29, #21] and the store, and in
   widened_sums_stored(), neither of whose arms widens free, the add
   and the sub joined in w0 and sxtw x0, w0 after the join, then the
   store.  In narrowed_stored() ldrb w0, [x29, #32] on each path
   and strb w0, [x1], the place in x1.  In summed_arm() the second
   arm's add w0, w1, w0 after x's reload, with no move after it.  In
   widened_called(), whose arms each store through a call's result in
   x0, the arms join in w1 and sxtw x0, w1 after the join; in
   joined_called() mov w0, w1 on each path. */
int conditional(int a, int b, int c)
{
    return a ? b : c;
}

int g;

int
stepped(int a, int b, int *p, int i)
{
        return a ? p[i]++ : b;
}

void
stored(int a, int b, int c, int d)
{
        g = a ? b % c : d;
}

int
summed(int a, int b, int c, int d, int e)
{
        return (a ? b % c : d) + e;
}

long
widened(int a, int b, int c)
{
        return a ? b % c : c;
}

long
widened_constants(int a)
{
        return a ? 4 : -5;
}

long
widened_quotients(int a, int b, int c)
{
        return a ? b / c : c / b;
}

long h;

void
widened_stored(int a, int b, int c)
{
        h = a ? b % c : c;
}

int
summed_arm(int a, int x, int d)
{
        return a ? 0 : x + (d ? 1 : 2);
}

long
widened_step(int a, int *p, int b)
{
        return a ? (*p)++ : b;
}

long
widened_chars(int a, char *p, char b)
{
        return a ? (*p)++ : b;
}

char
narrowed_stored(int a, char *p, char b)
{
        return a ? b : (*p = b);
}

int *first(void);
int *second(void);

long
widened_called(int a, int b)
{
        return a ? (*first() = b) : (*second() = b);
}

int
joined_called(int a, int b)
{
        return a ? (*first() = b) : (*second() = b);
}

void
widened_chars_stored(int a, signed char b, signed char c)
{
        h = a ? b : c;
}

void
widened_sums_stored(int a, int b, int c)
{
        h = a ? b + c : b - c;
}
