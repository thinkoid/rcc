/* backend.md 6.3 and aarch64.md 6.3, (T) a, and chapter 4 of each, a
   register variable: a signed char held in a register is sign
   extended when it is widened, a call's result after the call and a
   register variable at each write, and its read writes nothing more.
   On x86-64 call_byte() is call next@PLT and movsbl %al, %eax, and
   call_byte_long() the same with movsbq %al, %rax; register_byte()
   extends the unwritten variable at its declaration, movsbl %bl, %ebx,
   then the assignment's, movsbl %al, %ebx after the load of x, and
   c + 1 is movl %ebx, %eax and addl $1, %eax; register_call_byte() is
   movsbl %al, %ebx after the call, and the return movl %ebx, %eax.  On
   aarch64, where plain char is unsigned and a char of the other cost
   programs is extended with uxtb, call_byte() is bl next and sxtb w0,
   w0, and call_byte_long() sxtb x0, w0; register_byte() is sxtb w19,
   w19 at the declaration, sxtb w19, w0 after the load of x, and add w0,
   w19, #1, the read the register as it stands; register_call_byte() is
   sxtb w19, w0 after the bl, and the return mov w0, w19. */
signed char next(void);

int
call_byte(void)
{
        return next();
}

long
call_byte_long(void)
{
        return next();
}

int
register_byte(int x)
{
        register signed char c;

        c = x;
        return c + 1;
}

int
register_call_byte(void)
{
        register signed char c = next();

        return c;
}
