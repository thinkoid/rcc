/* GNU C's statement expression with a label and a goto in its block
   (shape.md chapter 5, backend.md 6.7): a loop built of goto, laid
   out as written, as in a function's body (goto).  a is held beside
   the block, spilled before its first statement as at a branch, and
   read in its slot after the value.  The label and the jump back stand
   at boundaries of statements, where no expression register holds a
   value, and the spilled a stays in its slot throughout, so neither
   writes anything for it.

   On x86-64 the listing must show a's load, movl -4(%rbp), %eax, and
   its spill, movq %rax, -16(%rbp); r's store, movl $0, -20(%rbp);
   .Lcounted.2 with no .p2align before it; the test of n-- and
   jle .Lcounted.3; addl $1, -20(%rbp) and jmp .Lcounted.2 back; then
   .Lcounted.3, r's load and addl -16(%rbp), %eax.

   On aarch64 the listing must show a's load, ldr w0, [x29, #16], and
   its spill, str x0, [x29, #24]; str wzr, [x29, #32] for r;
   .Lcounted.2; the test of n-- and b.le .Lcounted.3; r's increment
   and b .Lcounted.2 back; then .Lcounted.3, r's load, the reload
   ldr x1, [x29, #24] and add w0, w1, w0. */
int
counted(int a, int n)
{
        return a + ({
                       int r = 0;
               again:
                       if (n-- > 0) {
                               r++;
                               goto again;
                       }
                       r;
               });
}
