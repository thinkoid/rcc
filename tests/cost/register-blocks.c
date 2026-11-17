/* backend.md chapter 4 and aarch64.md chapter 4, register variables in
   blocks: n and s at the function's head, a and b each in a block of
   their own, two siblings, and c declared after both blocks.  a and b
   are never live at once and share one register.  c is live from the
   entry into the function's block, 6.2.4p6, so through both blocks, and
   takes the register above theirs.  The function takes four registers,
   one more than are ever live at once.

   On x86-64 the four are rbx, r12, r13 and r14, and the listing must
   show the four pushed in the prologue and popped in reverse in the
   epilogue, n moved from edi into ebx, s set with movl $0, %r12d, a and
   then b each computed in r13d by the multiply that reads n, imull $2,
   %ebx, %r13d, each sum the add on s's register, addl %r13d, %r12d,
   and c computed in r14d, movl %r12d, %r14d and addl $1, %r14d, with
   no mov after any of them.

   On aarch64 the four are x19, x20, x21 and x22, and the listing must
   show them stored to the save area by stp x19, x20 and stp x21, x22
   and loaded back by two ldp, n moved from w0 into w19, s set with
   mov w20, #0, a and then b each computed into w21, mul w21, w19, w0,
   each sum into w20, add w20, w20, w21, and c into w22, add w22, w20,
   #1, with no mov after any of them. */
int
blocks(register int n)
{
        register int s = 0;
        {
                register int a = n * 2;
                s = s + a;
        }
        {
                register int b = n * 3;
                s = s + b;
        }
        register int c = s + 1;
        return c;
}
