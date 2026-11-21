/* A variable length array in a type name, the operand of sizeof: rcc
   does not compile it yet. */
unsigned long
f(int n)
{
        return sizeof(char[n]);
}

int
main(void)
{
        return f(1) != 1;
}
