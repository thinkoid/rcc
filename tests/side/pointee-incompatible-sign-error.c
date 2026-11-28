/* A pointer to int and a pointer to unsigned point to incompatible
   types, an error at the argument. */
void g(unsigned *p);

void
f(int *p)
{
        g(p);
}
