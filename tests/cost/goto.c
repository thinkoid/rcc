/* backend.md 7.2 and aarch64.md 7.2, goto and labels: a loop built of
   goto, laid out as written.  A label the source names is one more
   label of the function, numbered when c1 first meets it: again at its
   definition, .Lcount.2, and done at the goto to it, .Lcount.4, after
   the if's label; the listing shows no C name.  No label of the
   source's is aligned, though a goto jumps back to again.

   On x86-64 the listing must show .Lcount.2 with no .p2align before
   it; the counter loaded and compared with cmpl %ecx, %eax, the jl past
   the goto and its jmp .Lcount.4; the counter incremented with
   addl $1, %eax and stored to -8(%rbp), and jmp .Lcount.2 back; then
   .Lcount.4 and the return.

   On aarch64 the listing must show the same shape: cmp w0, w1, b.lt
   past the goto and b .Lcount.4; the increment with add w0, w0, #1 and
   the store to [x29, #20]; b .Lcount.2 back. */
int
count(int n)
{
        int i = 0;
again:
        if (i >= n)
                goto done;
        i = i + 1;
        goto again;
done:
        return i;
}
