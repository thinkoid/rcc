/* An empty subscript is an error at the ]. */
int
f(int *p)
{
        return p[];
}
