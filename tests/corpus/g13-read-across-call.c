/* G13: left to right. In x + f(x++) the left operand reads x before
   the argument writes it. c1 gives x ebx; the write of x++ first
   copies the held read into eax, the argument is stored to its slot,
   and the copy is spilled before the call and reloaded after it. sum
   returns the old x, 5, plus f(5), 105: exit status 110. The
   expression is undefined in ISO C (6.5.1p2), since x is read and
   written unsequenced. clang -O2 writes sum as leal 100(,%rdi,2),
   %eax, twice x plus 100: 110. */
int f(int n)
{
    return n + 100;
}

int sum(register int x)
{
    return x + f(x++);
}

int main(void)
{
    return sum(5);
}
