/* * of a call of an int result is an error at the *. */
int g(void);

int
f(void)
{
        return *g();
}
