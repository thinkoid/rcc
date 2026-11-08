/* backend.md 6.3, ++p and p -= n on an int pointer: the constant
   times the size is the immediate.  The listing must show ++p as
   addq $4, %rax and p -= 2 as subq $8, %rax, each between the load of
   p from its slot and the store back. */
int *step(int *p)
{
    ++p;
    p -= 2;
    return p;
}
