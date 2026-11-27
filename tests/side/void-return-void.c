/* return of a void call in a void function, which 6.8.7.5p1 forbids and
   GNU C allows: rcc warns at the return and writes the call, then the
   jump to the epilogue. */
void h(void);

void
g(void)
{
        return h();
}
