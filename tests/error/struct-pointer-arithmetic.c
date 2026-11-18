/* Arithmetic on a pointer to an incomplete struct: the step is the
   struct's size, which an incomplete struct does not have, an error. */
struct point;
int adjacent(struct point *p, struct point *q)
{
    return p + 1 == q;
}
int main(void)
{
    return adjacent(0, 0);
}
