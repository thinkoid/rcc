/* A cast is to a scalar type, 6.5.5p2: an array's is the error. */
int
f(int x)
{
        (int[3]) x;
        return 0;
}
