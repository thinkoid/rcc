/* Two functions' names compared by <, which takes no pointer to a
   function, 6.5.9p2. */
int g(void);

int
f(void)
{
        return g < g;
}
