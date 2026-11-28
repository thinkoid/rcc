/* A pointer to long and a pointer to long long point to incompatible
   types, though both are of 64 bits, an error at the value returned. */
long long *
f(long *p)
{
        return p;
}
