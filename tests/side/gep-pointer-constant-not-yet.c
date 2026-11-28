/* A sum on a pointer constant, in either order, by a variable and by a
   constant. */
int *
f(int i)
{
        return (int *) 4096 + i;
}

int *
g(void)
{
        return 2 + (int *) 4096;
}
