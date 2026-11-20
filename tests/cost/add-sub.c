/* backend.md 6.3 and aarch64.md 6.3, a + b and a - b: add and sub.

   On x86-64 the listing must show a loaded into eax, then
   addl -8(%rbp), %eax in add() and subl -8(%rbp), %eax in sub(), b's
   slot the instruction's operand (backend.md 6.1), and nothing else
   between the load and the jmp to the epilogue label.

   On aarch64 the listing must show a loaded into w0 and b into w1, then
   add w0, w0, w1 in add() and sub w0, w0, w1 in sub(), and nothing else
   between the loads and the b to the epilogue label. */
int
add(int a, int b)
{
        return a + b;
}

int
sub(int a, int b)
{
        return a - b;
}
