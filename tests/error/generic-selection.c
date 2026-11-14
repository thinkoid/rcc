/* A generic selection, which is not yet. */
int main(void)
{
    return _Generic(0, int: 1, default: 0);
}
