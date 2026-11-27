/* Fewer arguments than parameters of a char and a long: the error is
   at the ')' that ends them. */
int g(char a, long b);

int
f(void)
{
        return g(1);
}
