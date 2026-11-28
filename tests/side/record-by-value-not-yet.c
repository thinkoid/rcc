/* A record result of a definition returns by value in no rule rcc has
   yet, in rcc's words at the function's name. */
struct s
{
        int a;
};

struct s
f(struct s *p)
{
        return *p;
}
