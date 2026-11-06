/* Chapter 3, *p through a pointer parameter: a load or a store with
   align 1 in the IR (G11), which the listing cannot show, since x86
   has one mov for every alignment.  The listing must show load()
   reading through the pointer with mov (%rax),%eax, and store()
   writing with mov %ecx,(%rax). */
int load(int *p)
{
    return *p;
}

void store(int *p, int v)
{
    *p = v;
}
