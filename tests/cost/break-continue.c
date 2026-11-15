/* backend.md 7.2 and aarch64.md 7.2, break and continue: each a jump
   and nothing else, since no value is held from one statement to the
   next.  In the for, the continue jumps to the step and the break past
   the loop; in the while, whose condition is the constant 1, the break
   jumps past the loop.

   On x86-64 the listing must show, after cmpl $5, %eax, the jne past
   the continue and its jmp .Lsum.3 to the step's label; after
   cmpl $100, %eax, the jle past the break and its jmp .Lsum.4, the
   label past the for, which the while's .p2align 5 and head follow;
   and in the while, after cmpl $50, %eax, the jge past the break and
   its jmp .Lsum.8 past the while, then the jmp back to the head.

   On aarch64 the listing must show the same jumps as b: after
   cmp w0, #5 the b.ne and b .Lsum.3; after cmp w0, #100 the b.le and
   b .Lsum.4; after cmp w0, #50 the b.ge and b .Lsum.8; and the
   constant condition held, mov w0, #1, and tested with cbz w0. */
int
sum(int n)
{
        int s = 0;
        for (int i = 0; i < n; i = i + 1) {
                if (i == 5)
                        continue;
                if (s > 100)
                        break;
                s = s + i;
        }
        while (1) {
                s = s - 1;
                if (s < 50)
                        break;
        }
        return s;
}
