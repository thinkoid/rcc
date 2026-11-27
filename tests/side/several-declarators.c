/* One declaration may declare several functions, its declarators
   separated by commas, 6.7.1p1, and a function may be declared again
   with the type it has, any number of times, 6.7.1p5, before its
   definition and after it. */
int f(void), g(void);
int f(void);

int
f(void)
{
        return 1;
}

int f(void), g(void);
