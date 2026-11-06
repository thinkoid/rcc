/* Chapter 3, a[i]; chapter 4, the addressing-mode fold.  The gep of
   a[i] feeds the store and the load, and each is selected with the
   gep as its scaled-index address, the slot's offset as the
   displacement: the listing must show i loaded and widened with
   cltq before each access, the store and the load each through
   -0x14(%rsp,%rax,4), and no imul, shl or lea for the address. */
int subscript(int i, int v)
{
    int a[4];

    a[i] = v;
    return a[i];
}
