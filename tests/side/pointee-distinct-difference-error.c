/* The difference of a pointer to char and a pointer to short is an
   error at the operator, said before the pointees' sizes are read. */
long
f(char *p, short *q)
{
        return p - q;
}
