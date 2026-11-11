/* backend.md 6.3 and 6.6, x op= e: in the effect context the
   operation on the object, in a value context the load row, the op row
   and the store row.  The listing must show compound() loading b and
   adding it to x's slot with addl %eax, -12(%rbp); narrow() adding the
   low byte of b to c's slot with addb %al, -9(%rbp); and in_value(),
   whose value is returned, loading b, the right operand, first, then c
   with movsbl into eax, adding b in int with addl %ecx, %eax and
   storing the low byte back with movb %al. */
int compound(int a, int b)
{
    int x = a;
    x += b;
    return x;
}

int narrow(int a, int b)
{
    char c = a;
    c += b;
    return c;
}

int in_value(int a, int b)
{
    char c = a;
    return c += b;
}
