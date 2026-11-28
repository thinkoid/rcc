/* A bit-field's access is not yet. */
struct s
{
        int a : 3;
        int b;
};

int
f(struct s *p)
{
        return p->a;
}
