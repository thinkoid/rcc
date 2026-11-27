/* A void call as the argument of a long parameter: the error is at
   the argument's first token. */
void h(void);
int g(long a);

int
f(void)
{
        return g(h());
}
