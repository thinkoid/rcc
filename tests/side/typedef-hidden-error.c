/* An object of a block hides a typedef name: T y is an expression
   there. */
typedef int T;

int
f(void)
{
        int T = 1;
        T y = 2;
        return y;
}
