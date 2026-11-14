/* A local of a struct type: the struct is declared, its members
   skipped, and the object, which needs the struct's size, is not
   yet. */
int main(void)
{
    struct point { int x; int y; } p;

    return 0;
}
