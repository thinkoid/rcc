/* A parameter of the function type a typedef name names, adjusted to
   a pointer to the function, is not yet. */
typedef int F(int);

int
f(F p)
{
        return 0;
}
