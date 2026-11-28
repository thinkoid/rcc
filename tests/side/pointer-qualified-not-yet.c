/* A qualifier of a pointer is not yet, at the qualifier. */
int
f(void)
{
        int x;
        int *const p = &x;
        return 0;
}
