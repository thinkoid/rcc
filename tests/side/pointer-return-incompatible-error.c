/* A pointer to int returned as a pointer to a pointer is an error at
   the value. */
int **
f(int *p)
{
        return p;
}
