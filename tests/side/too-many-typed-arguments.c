/* More arguments than the one short parameter: the error is at the
   first token of the one past it. */
int g(short a);

int
f(void)
{
        return g(1, 2);
}
