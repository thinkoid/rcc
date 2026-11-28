/* A member access names its member by an identifier, 6.5.3.1p1. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        return p->;
}
