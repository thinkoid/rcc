/* backend.md 6.3, a + b and a - b: add and sub.  The listing must
   show a loaded into eax and b into ecx, then addl %ecx, %eax in
   add() and subl %ecx, %eax in sub(), and nothing else between the
   loads and the jmp to the epilogue label. */
int add(int a, int b)
{
    return a + b;
}

int sub(int a, int b)
{
    return a - b;
}
