/* backend.md 6.3, x op= e: the load row, the op row, the store row.
   The listing must show compound() loading x from its slot, adding b
   with addl %ecx, %eax and storing x back, all in int; and narrow()
   loading c with movsbl, adding b in int and storing the low byte
   back with movb %al, -9(%rbp). */
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
