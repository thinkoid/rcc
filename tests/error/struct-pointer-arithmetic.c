/* Arithmetic on a pointer to a struct: the step is the struct's size,
   which is not yet. */
struct point;

int adjacent(struct point *p, struct point *q)
{
    return p + 1 == q;
}

int main(void)
{
    return adjacent(0, 0);
}
