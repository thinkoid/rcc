/* A pointer to int passed for a pointer to a pointer is an error at the
   argument. */
int g(int **pp);

int
f(int *p)
{
        return g(p);
}
