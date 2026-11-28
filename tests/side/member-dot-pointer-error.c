/* The operand of . is a record, 6.5.3.4p1, and a pointer is none. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        return p.a;
}
