/* A function's name compared with an int by <=: a pointer and an
   integer, 6.5.9p2. */
int g(void);

int
f(int a)
{
        return a <= g;
}
