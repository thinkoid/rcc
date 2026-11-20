/* return g(); of a void g in a void function: 6.8.7.5p1 allows no
   expression there, and GNU C takes a void one, so rcc takes it, with a
   warning, and the program compiles. */
void
g(void)
{
}

void
f(void)
{
        return g();
}

int
main(void)
{
        f();
        return 0;
}
