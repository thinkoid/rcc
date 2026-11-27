/* A parameter is in the scope the body's outermost block shares,
   6.9.2p10, so a nested block may hide it with a local of its own,
   which takes the next slot after the parameters' and the locals'
   before it; a parameter hides a function of its name declared at
   file scope, its own included.  A declaration's parameter names need
   not be the definition's, and may be left out: the definition's
   names are the body's. */
int g(int x, int);

int
g(int a, int b)
{
        int c = a;
        {
                int a = 7;
                c = c + a;
        }
        return c + a + b;
}

int
h(int g)
{
        return g;
}

int
self(int self)
{
        return self + 1;
}
