/* backend.md chapter 8, a char or short parameter or result: the
   argument is stored at its type's width and extended by the load
   into its register, and the callee extends its result.  The listing
   must show f() storing dil with movb to -1(%rbp) and si with movw to
   -4(%rbp), loading them back with movsbl and movswl, and extending
   its char result with movsbl %al, %eax before the jmp to the
   epilogue label; g() storing dil the same way, loading it with
   movzbl and extending its result with movzbl %al, %eax; and h()
   storing x to its two argument slots with movb %al, -5(%rbp) and
   movw %ax, -8(%rbp), loading them into edi and esi with movsbl and
   movswl, and writing nothing after the call, since f's result is
   valid to 32 bits as it comes. */
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
