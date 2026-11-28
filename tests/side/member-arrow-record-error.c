/* The operand of -> is a pointer, 6.5.3.4p2, and a record is none. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        return (*p)->a;
}
