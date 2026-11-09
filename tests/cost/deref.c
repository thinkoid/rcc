/* backend.md 6.3, *p through a pointer parameter: one mov, any
   address (G8, G11), since x86 has one mov for every alignment.  The
   listing must show load() loading p into rax with movq and reading
   through it with movl (%rax), %eax, and store() writing v with
   movl %ecx, (%rax).  store() has no return statement, so it falls
   into the epilogue label with no jmp. */
int load(int *p)
{
    return *p;
}

void store(int *p, int v)
{
    *p = v;
}
