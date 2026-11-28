/* An argument of a record parameter converts from that record alone,
   6.5.3.3p2, 6.5.17.2p1. */
struct s
{
        int a;
};
int g(struct s);

int
f(int n)
{
        return g(n);
}
