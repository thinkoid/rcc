/* A local that hides a function of its name, called: the local is no
   function. */
int g(void);

int
f(void)
{
        int g = 1;
        return g();
}
