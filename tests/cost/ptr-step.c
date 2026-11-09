/* backend.md 6.3 and 6.6, ++p and p -= n on an int pointer: the
   constant times the size is the immediate.  The listing must show
   step() writing ++p as addq $4, -8(%rbp) and p -= 2 as
   subq $8, -8(%rbp), on p's slot, with no load and no store; and
   in_value(), whose value is returned, writing p -= 2 as subq $8, %rax
   between the load of p from its slot and the store back. */
int *step(int *p)
{
    ++p;
    p -= 2;
    return p;
}

int *in_value(int *p)
{
    return p -= 2;
}
