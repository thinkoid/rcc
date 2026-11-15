/* A call through a pointer to a function whose result is long double:
   the pointer is accepted, and the call, which makes the object it
   returns, is not yet, as for a named call. */
void
call(long double (*p)(int))
{
        p(1);
}

int
main(void)
{
        return 0;
}
