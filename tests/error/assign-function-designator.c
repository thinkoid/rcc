/* *p on a pointer to a function is a function designator, which
   designates no object, 6.3.2.1p1, p4: it is not assignable. */
void
replace(void (*p)(int), void (*q)(int))
{
        *p = q;
}

int
main(void)
{
        return 0;
}
