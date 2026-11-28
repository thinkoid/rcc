/* A member of a record's value, which no record has here, is not yet. */
struct s
{
        int a;
};

int
f(int c, struct s *p, struct s *q)
{
        return (c ? *p : *q).a;
}
