/* The call instruction is call f@PLT on x86-64 and bl f on aarch64,
   whatever the function: defined in the unit, static, only declared,
   the C library's, or the function itself.  () declares no parameter,
   as (void) does, and nothing is written before the call. */
int putchar(int c);
int declared(void);
int empty();

int
defined(void)
{
        return 1;
}

static int
internal(void)
{
        return 2;
}

int
calls(void)
{
        defined();
        internal();
        declared();
        empty();
        return putchar(65);
}

int
self(int a)
{
        return self(a);
}
