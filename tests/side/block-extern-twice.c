/* One block declares the same entity extern twice, 6.7.1p4. */
int
f(void)
{
        extern int e;
        extern int e;
        return e;
}
