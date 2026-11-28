/* The address of an object of static storage is not yet, at the &. */
int g;

int
f(void)
{
        return &g == 0;
}
