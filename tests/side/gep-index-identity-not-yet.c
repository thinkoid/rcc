/* An index's cast to its own type under an operator's identity is still
   the cast. */
int
f(int *p, unsigned u)
{
        return p[+u + 0];
}
