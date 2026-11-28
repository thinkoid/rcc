/* A tag declared by struct is no union's, 6.7.3.4p2. */
struct s
{
        int a;
};
union s *f(void);
