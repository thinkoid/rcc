/* A record argument is not yet at its first token, once its conversion
   is judged, in rcc's words. */
struct s
{
        int a;
};
int g(int, struct s);

int
f(struct s *p)
{
        return g(1, *p);
}
