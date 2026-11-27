/* A block's extern object of a function's name: the entity is the
   function, of another kind. */
int g(void);
int
f(void)
{
        extern int g;
        return g;
}
