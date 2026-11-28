/* A pointer to char and a pointer to unsigned char point to
   incompatible types, an error at the expression assigned. */
void
f(char *p, unsigned char *q)
{
        q = p;
}
