/* The unit writes its functions in the order of each one's first
   declaration: a declaration makes the function's entry, and a later
   declaration and the definition find it again.  a and b are declared
   first and defined after c, so the listing is a, b, c, whatever the
   order of the definitions; z is declared and never defined, and
   writes nothing, no .globl and no label. */
int a(void);
int b(void);
int z(void);

int
c(void)
{
        return 3;
}

int
b(void)
{
        return 2;
}

int
a(void)
{
        return 1;
}
