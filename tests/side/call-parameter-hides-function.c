/* A parameter that hides a function of its name, called in
   parentheses, which the error sees through. */
int h(void);

int
g(int h)
{
        return (h) ();
}
