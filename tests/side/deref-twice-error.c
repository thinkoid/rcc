/* ** of a pointer to int is an error at the first *. */
int
f(int *p)
{
        return **p;
}
