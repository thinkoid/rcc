/* A record and an integer arm of ?: meet in none of 6.5.16p3's pairs. */
struct s
{
        int a;
};

void
f(int c, struct s *p)
{
        c ? *p : 1;
}
