/* A function's name less an int: its pointer points to no object,
   6.5.7p3. */
int g(void);

int
f(void)
{
        return g - 1;
}
