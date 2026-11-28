/* A call of a function returning a record is not yet at its '(', before
   its arguments, in rcc's words. */
struct s
{
        int a;
};
struct s g(int);

int
f(void)
{
        g(1);
        return 0;
}
