/* backend.md 7.1 and aarch64.md 7.1, a pointer as a condition, and 6.3
   of both, p == 0 as a value.  Beside them a conditional of a null
   pointer constant (void *) 0 and an int *, whose type is the int *,
   6.5.16p7: through() reads an int through it, and stepped() adds 1 to
   it as to an int *.

   On x86-64 the listing must show test() comparing the pointer in its
   slot, cmpq $0, -8(%rbp), once and branching with je, then each
   return's movl and jmp to the epilogue label, the last one included;
   and equal() comparing the pointer in its slot, cmpq $0, -8(%rbp),
   with no load, and sete, widened with movzbl %al, %eax.  through()
   joins movq $0, %rax and the load of p in rax, then reads the int,
   movl (%rax), %eax; stepped() joins them the same and adds an int's
   size, addq $4, %rax.

   On aarch64 the listing must show test() testing the pointer with
   cbz x0 once, then each return's mov and b to the epilogue label, the
   last one included; and equal() comparing with cmp x0, #0 and
   cset w0, eq.  through() joins mov x0, #0 and the load of p in x0,
   then reads the int, ldr w0, [x0]; stepped() joins them the same and
   adds an int's size, add x0, x0, #4. */
int
test(int *p)
{
        if (p)
                return 1;
        return 0;
}

int
equal(int *p)
{
        return p == 0;
}

int
through(int c, int *p)
{
        return *(c ? (void *) 0 : p);
}

int *
stepped(int c, int *p)
{
        return (c ? (void *) 0 : p) + 1;
}
