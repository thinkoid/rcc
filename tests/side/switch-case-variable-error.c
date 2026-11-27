/* A case's expression is an integer constant expression, 6.8.5.3p3,
   said at its first token. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case b: c = 1;
        }
        return c;
}
