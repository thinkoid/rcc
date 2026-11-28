/* sizeof of void is an error at the sizeof. */
int
f(void)
{
        return sizeof(void);
}
