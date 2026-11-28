/* A record is no type of a cast, 6.5.5p2. */
struct s
{
        int a;
};

void
f(int x)
{
        (struct s) x;
}
