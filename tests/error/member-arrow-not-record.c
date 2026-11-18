/* -> on a pointer to an int, which is no struct or union, 6.5.3.4p2. */
int
first(int *p)
{
        return p->x;
}

int
main(void)
{
        return 0;
}
