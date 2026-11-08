/* backend.md chapter 8, f(a, b): each argument a hidden local of the
   call.  The listing must show the subq $16, %rsp of a function that
   calls, the parameters stored to their slots, b and then a each
   loaded into eax and stored to an argument slot of its own,
   -12(%rbp) and -16(%rbp), those two slots loaded into edi and esi,
   call f@PLT, and the result left in eax. */
int f(int a, int b);

int call(int a, int b)
{
    return f(b, a);
}
