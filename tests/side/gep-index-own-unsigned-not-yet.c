/* An index of an unsigned object under its unary plus, a conversion to
   its own type, is not yet, at the +. */
int
f(int *p, unsigned u)
{
        return p[+u];
}
