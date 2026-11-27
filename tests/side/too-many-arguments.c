/* More arguments than parameters, 6.5.3.3p2: the error is at the first
   argument past them, once, after it is read. */
int g(int a);

int
f(void)
{
        return g(1, 2, 3);
}
