/* A function type is no scalar type, and no cast's, 6.5.5p2. */
typedef int F(int);

int
f(int x)
{
        return (F) x;
}
