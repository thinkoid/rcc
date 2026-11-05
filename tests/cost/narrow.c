/* Chapter 3, a char or short parameter or result: i32 in the
   signature, trunc at entry, sext or zext before the call and before
   ret, trunc after the call.  The listing must show f() storing the
   low byte of edi and the low word of esi to their slots, its result
   sign-extended with movsbl before ret; g() storing the low byte of
   edi and its result zero-extended with movzbl before ret; and h()
   extending the two arguments with movsbl and movswl before the call
   and reading the result with one movsbl after it: the trunc is the
   low byte of eax, no instruction, and the movsbl is the conversion
   to int for h's own return. */
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
