/* An object and a parameter of a prototype of one name: the
   parameter's scope is the prototype's, 6.2.1p4. */
int x;
int g(int x);
int
f(void)
{
        return g(x);
}
