/* backend.md 6.3 and aarch64.md 6.3, *p through a pointer parameter:
   one load or one store, any address (G8, G11).  store() has no return
   statement, so it falls into the epilogue label with no jump.

   On x86-64, which has one mov for every alignment, the listing must
   show load() loading p into rax with movq and reading through it with
   movl (%rax), %eax, and store() writing v with movl %ecx, (%rax) and
   no jmp after it. */
int load(int *p)
{
    return *p;
}

void store(int *p, int v)
{
    *p = v;
}
