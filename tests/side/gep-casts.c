/* A cast between pointer types over a sum: one cast is read through to
   the sum, its stride with the access of the cast's pointee, and a
   second hides the sum. */
int
under_one(int **pp, int i)
{
        return *(int *) (pp + i);
}

int *
under_one_pointer(int ***ppp, int i)
{
        return *(int **) (ppp + i);
}

int *
under_two(int ***ppp, int i)
{
        return *(int **) (int *) (ppp + i);
}

int
stored(int ***ppp, int i)
{
        int *q;
        q = 0;
        *(int **) (ppp + i) = q;
        *(ppp + i) = (int **) q;
        return 0;
}

int
same_type(int *p, int i)
{
        return *(int *) (p + i) + *(int *) (int **) (p + i);
}
