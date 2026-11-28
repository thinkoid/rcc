/* Two pointers to different types are no difference, an error at the -. */
long
f(int *p, int **q)
{
        return p - q;
}
