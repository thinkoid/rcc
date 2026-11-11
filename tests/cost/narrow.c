/* backend.md chapter 8, a char or short parameter or result: the
   argument is stored at its type's width and extended by the load
   into its register, and the caller extends the result.  The listing
   must show f() storing dil with movb to -1(%rbp) and si with movw to
   -4(%rbp), loading them back with movsbl and movswl, and writing
   nothing between its addl and the jmp to the epilogue label; g()
   storing dil the same way, loading it with movzbl, and writing
   nothing after its addl either; and h() storing x to its two
   argument slots with movb %al, -5(%rbp) and movw %ax, -8(%rbp),
   loading them into edi and esi with movsbl and movswl, and extending
   f's result with movsbl %al, %eax after the call, since the result
   is valid to 8 bits as it comes. */
char f(char c, short s)
{
    return c / 4 + s;
}

unsigned char g(unsigned char u)
{
    return u + 1;
}

int h(int x)
{
    return f(x, x);
}
