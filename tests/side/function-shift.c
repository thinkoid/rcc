/* A function's name as a shift's count, 6.5.8p2. */
int g(void);

int
f(void)
{
        return 1 << g;
}
