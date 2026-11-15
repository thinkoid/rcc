/* A call through a pointer to a function whose parameter is long
   double: the pointer is accepted, and the argument, converted to the
   parameter's type, is not yet. */
void
call(void (*p)(long double))
{
        p(1);
}

int
main(void)
{
        return 0;
}
