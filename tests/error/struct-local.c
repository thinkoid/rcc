/* A local of a struct type: the struct is laid out, and the object,
   which c1 does not make yet, is not yet. */
int main(void)
{
    struct point { int x; int y; } p;
    return 0;
}
