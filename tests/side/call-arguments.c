/* The arguments of a call.  Every held value is spilled first, here
   none; then each argument, left to right and complete before the next,
   takes a 4-byte slot of its own, after the frame's slots so far, and
   is stored there: a constant is the immediate into the slot on x86-64,
   movl $-1, -16(%rbp), and its register's store on aarch64, str wzr for
   0; anything else is computed in eax or w0, every register being free,
   and stored.  Then the slots are loaded into the argument registers
   and the call made.  Every call has slots of its own.  A nested call's
   slots are taken after the outer argument's, and its result is
   stored to that slot.  An argument that divides or shifts finds rax,
   rdx and rcx free. */
int g(int a);
int g3(int a, int b, int c);

int
order(void)
{
        int a = 0;
        return g3(a = 1, a = a + 1, a);
}

int
three(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return g3(a, b, c);
}

int
constants(void)
{
        return g3(-1, 100000, 2147483647);
}

int
zero(void)
{
        return g3(0, 1, 0);
}

int
twice(void)
{
        int a = 1;
        g(a);
        return g(a);
}

int
nested(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return g3(a, g(b), c);
}

int
pinned(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return g3(a / b, a % b, a << c);
}
