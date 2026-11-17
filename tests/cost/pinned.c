/* backend.md 6.5 and aarch64.md 6.5, the pinned instructions: on
   x86-64 a division wants its dividend in rax and writes rdx, and a
   shift by a variable count wants its count in cl.  The instruction's
   own node chooses for its operands: a divisor and its address stay
   out of rax and rdx, a shift's left operand out of rcx, the place of
   /= out of rax and rdx and of <<= out of rcx, and a dividend spilled
   is reloaded into rax.  A node whose right operand's top operator is
   a division or such a shift holds its left operand, or an assignment
   its place, out of the registers that instruction pins.  fill,
   shifted and shifted_through store a remainder into a global array,
   a shift into a local one and a shift through a pointer;
   divided_in_place and shifted_in_place divide and shift an element
   through a pointer in place; remainder_call divides by an element of
   an array a call returns; less compares with a quotient, and
   subscript reads an element by one.

   On x86-64 the listing must show no move between registers but the
   return's, movl %edx, %eax in remainder_call: in fill the array's
   address in rcx and the index in rsi, the dividend in eax,
   shrl $16, %eax, the divisor in edi, xorl %edx, %edx, divl %edi and
   the store of the remainder from edx, movl %edx, (%rcx,%rsi,4), since
   a remainder's value is the value in rdx (backend.md 6.5); in shifted
   the index in rax, x in edx and the count in ecx, shll %cl, %edx and
   movl %edx, -44(%rbp,%rax,4); in shifted_through p in rax, the index
   in rdx, x in esi, the count in ecx and movl %esi, (%rax,%rdx,4); in
   divided_in_place p in rcx, the index in rsi, d in edi, the element
   loaded into eax, cltd, idivl %edi and the store from eax; in
   shifted_in_place p in rax, the index in rdx, the count in ecx and
   shll %cl, (%rax,%rdx,4); in remainder_call the call's result, the
   array's address, moved out of rax, movq %rax, %rsi, x reloaded from
   its spill slot into rax, movq -16(%rbp), %rax, cltd and
   idivl (%rsi,%rcx,4); in less x in ecx and y in eax, cltd,
   idivl -12(%rbp) and cmpl %eax, %ecx, then setl %al and
   movzbl %al, %eax, in the register the return names (backend.md
   6.1); in subscript p in rcx and y in
   eax, cltd, idivl -16(%rbp), movslq %eax, %rax and
   movl (%rcx,%rax,4), %eax.

   On aarch64, where no instruction pins a register, the listing is the
   operations on the registers the loads took, and the rows are not
   reached: udiv w4, w2, w3 and msub w2, w4, w3, w2 in fill, lsl w2, w2,
   w3 in shifted and shifted_through, sdiv w3, w3, w2 in
   divided_in_place and lsl w3, w3, w2 in shifted_in_place, each
   between the element's load and its store, sdiv and msub on the
   reloaded x in remainder_call and its result moved into w0, mov w0,
   w1, sdiv w1, w1, w2 in less and subscript. */
int a[8];

int *f(void);

void
fill(int i, unsigned seed)
{
        a[i] = (seed >> 16) % 256;
}

void
shifted(int i, int x, int n)
{
        int b[8];

        b[i] = x << n;
}

void
shifted_through(int *p, int i, int x, int n)
{
        p[i] = x << n;
}

void
divided_in_place(int *p, int i, int d)
{
        p[i] /= d;
}

void
shifted_in_place(int *p, int i, int n)
{
        p[i] <<= n;
}

int
remainder_call(int x, int i)
{
        return x % f()[i];
}

int
less(int x, int y, int z)
{
        return x < y / z;
}

int
subscript(int *p, int y, int z)
{
        return p[y / z];
}
