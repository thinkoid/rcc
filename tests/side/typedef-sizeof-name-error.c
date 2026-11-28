/* sizeof of a typedef name needs its parentheses: without them the
   name is an expression. */
typedef int T;

unsigned long
f(void)
{
        return sizeof T;
}
