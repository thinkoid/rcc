/* A typedef name is no expression, 6.5.2p2. */
typedef int T;

int
f(int x)
{
        return T(x);
}
