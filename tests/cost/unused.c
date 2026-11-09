/* backend.md 6.1, the rule: an expression statement is written like
   any other expression, though its value is unused.  The listing must
   show a + 0 as a loaded into eax and addl $0, %eax, and a / b as a
   and b loaded, cltd and idivl %ecx, then movl $0, %eax for the
   return: the division is performed, so it faults when b is 0 or when
   a is INT_MIN and b is -1 (G4). */
int unused(int a, int b)
{
    a + 0;
    a / b;
    return 0;
}
