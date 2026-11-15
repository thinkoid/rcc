/* &*p on a pointer to a function is p's value, as if both operators
   were omitted, but it is not an lvalue, 6.5.4.2p3: it is not
   assignable. */
void
replace(void (*p)(int), void (*q)(int))
{
        &*p = q;
}

int
main(void)
{
        return 0;
}
