/* backend.md 7.2 and aarch64.md 7.2, do: the body first, the test at
   the bottom, where it is written, and a conditional jump back while it
   holds; one test an iteration and no jump out.

   On x86-64 the listing must show .p2align 5 before the body's label,
   the loop's head; the counter loaded, incremented with addl $1, %eax
   and stored to -8(%rbp); then under a label of its own, which a
   continue would jump to, the counter loaded and compared with n in
   its slot, cmpl -4(%rbp), %eax, and the jl back to the head; and no
   label past the test, which no break jumps to.

   On aarch64 the listing must show, with no alignment directive before
   the head, the counter loaded, incremented with add w0, w0, #1 and
   stored to [x29, #20]; then under the condition's label the counter
   loaded and compared with cmp w0, w1 and the b.lt back to the head;
   and no label past the test. */
int
count(int n)
{
        int i = 0;
        do
                i = i + 1;
        while (i < n);
        return i;
}
