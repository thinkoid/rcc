/* backend.md 6.3 and 6.6 and aarch64.md 6.3 and 6.6, x op= e: e first,
   then x.  In the effect context the operation is on the object where
   the machine has that form; otherwise, and in a value context, it is
   the load row, the op row and the store row.

   On x86-64 the listing must show compound() loading b and adding it to
   x's slot with addl %eax, -12(%rbp); narrow() adding the low byte of b
   to c's slot with addb %al, -9(%rbp); and in_value(), whose value is
   returned, loading b, the right operand, first, then c with movsbl
   into eax, adding b in int with addl %ecx, %eax and storing the low
   byte back with movb %al.

   On aarch64, which has no memory form, the listing must show
   compound() loading b into w0, then x into w1, add w1, w1, w0 and
   str w1 back to x's slot; narrow() the same on c, ldrb w1, the add and
   strb w1; and in_value() the same up to the strb, then uxtb w1, w1,
   the char value widened for the return, and mov w0, w1. */
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
