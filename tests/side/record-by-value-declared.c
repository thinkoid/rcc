/* A declaration that names a function of a record parameter or result
   makes no object of either, and is taken. */
struct s;
int f(struct s x);
struct s g(struct s, int);
struct t
{
        long l;
};
struct t h(void);

int
k(int a)
{
        return a;
}
