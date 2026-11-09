/* backend.md 6.3, a && b and a || b as a value: the jump code of
   chapter 7 into eax, with no setcc.  Each operand is compared with
   cmpl $0, %eax, and the right one is not loaded when the left
   decides.  The listing must show both() jumping with je to
   movl $0, %eax when a or b is 0, and otherwise writing
   movl $1, %eax; and either() jumping with jne past b's test to
   movl $1, %eax when a is not 0, and b's test jumping with je to
   movl $0, %eax. */
int both(int a, int b)
{
    return a && b;
}

int either(int a, int b)
{
    return a || b;
}
