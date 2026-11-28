/* A comma whose right operand is an int through a pointer, on the right
   of an operator: x86-64 reads it in memory as the instruction's
   operand after the comma's left operand, addl (%rcx), %eax and idivl
   (%rcx), rcc's operand_of(). */
int
arithmetic(int *p, int a, int b)
{
        return a + (b, *p);
}

int
division(int *p, int a, int b)
{
        return a / (b, *p);
}

int
shift(int *p, int a, int b)
{
        return a << (b, *p);
}

int
comparison(int *p, int a, int b)
{
        return a < (b, *p);
}
