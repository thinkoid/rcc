/* G16: a call goes out as written.  wide is defined with an int result
   and called through a pointer converted to a function of another
   type, one that returns unsigned char.  The call is made with the call
   site's types: the result is read as an unsigned char, the low byte of
   the 300 wide returns, and main returns 44.  The call is undefined in
   ISO C (6.5.3.3p7), since the function is defined with a type that is
   not compatible with the type the pointer points to.  Nothing checks
   the pointer against the function it came from.

   On x86-64 wide's address is leaq wide(%rip), %rax; the cast writes
   nothing; the call is call *-16(%rbp) through the callee's slot, and
   the caller extends the result as the call site's unsigned char,
   movzbl %al, %eax.

   On aarch64 wide's address is adrp x0, wide and add x0, x0,
   :lo12:wide; the cast writes nothing; the call is ldr x16,
   [x29, #24] and blr x16, and the caller extends the result as an
   unsigned char, uxtb w0, w0.

   clang -O2 calls wide and extends the result as the call site's type
   too, and also returns 44. */
int
wide(void)
{
        return 300;
}

int
main(void)
{
        unsigned char (*narrow)(void) = (unsigned char (*)(void)) wide;

        return narrow();
}
