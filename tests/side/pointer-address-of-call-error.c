/* & of a call is an error at the &. */
int g(void);

int
f(void)
{
        int *p = &g();
        return 0;
}
