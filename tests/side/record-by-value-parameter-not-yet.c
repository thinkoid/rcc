/* A record parameter of a definition passes by value in no rule rcc has
   yet, in rcc's words at its name. */
struct s
{
        int a;
};

int
f(int n, struct s x)
{
        return n;
}
