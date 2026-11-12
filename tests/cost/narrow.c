/* backend.md chapter 8 and aarch64.md chapter 8, a char or short
   parameter or result: the argument is stored at its type's width and
   extended by the load into its register, and the caller extends the
   result.

   On x86-64 the listing must show f() storing dil with movb to -1(%rbp)
   and si with movw to -4(%rbp), loading them back with movsbl and
   movswl, and writing nothing between its addl and the jmp to the
   epilogue label; g() storing dil the same way, loading it with movzbl,
   and writing nothing after its addl either; and h() storing x to its
   two argument slots with movb %al, -5(%rbp) and movw %ax, -8(%rbp),
   loading them into edi and esi with movsbl and movswl, and extending
   f's result with movsbl %al, %eax after the call, since the result is
   valid to 8 bits as it comes.

   On aarch64, where plain char is unsigned (aarch64.md chapter 3), the
   listing must show f() storing w0 with strb to [x29, #16] and w1 with
   strh to [x29, #18], loading them back with ldrb and ldrsh, and
   writing nothing between its add and the b to the epilogue label; g()
   storing w0 the same way, loading it with ldrb, and writing nothing
   after its add either; and h() storing x to its two argument slots
   with strb w0, [x29, #20] and strh w0, [x29, #22], loading them
   into w0 and w1 with ldrb and ldrsh, and extending f's result with
   uxtb w0, w0 after the bl, since the result is valid to 8 bits as it
   comes. */
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
