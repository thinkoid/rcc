/* A member access on an int, which is no struct or union, 6.5.3.4p1. */
int main(void)
{
    int x = 1;

    return x.y;
}
