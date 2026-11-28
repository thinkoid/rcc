/* Pointers to pointers to two integer types point to incompatible
   types, an error at the initializer. */
void
f(char **p)
{
        short **q = p;
}
